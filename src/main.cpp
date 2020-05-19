#include <iostream>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <vector>
#include <map>

#include <cstdio>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#	include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#	include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#	pragma comment(lib, "legacy_stdio_definitions")
#endif

typedef	bool					Bool;

typedef	unsigned char			UInt8;
typedef	unsigned short			UInt16;
typedef	unsigned int			UInt32;
typedef	unsigned long long		UInt64;

typedef	char					Int8;
typedef	short					Int16;
typedef	int						Int32;
typedef	long long				Int64;

typedef	float					f32;
typedef	double					f64;

inline Bool		operator==(const ImVec2& lhs, const ImVec2& rhs){ return lhs.x == rhs.x && lhs.y == rhs.y; }
inline Bool		operator!=(const ImVec2& lhs, const ImVec2& rhs){ return lhs.x != rhs.x || lhs.y != rhs.y; }
inline ImVec2	operator+(const ImVec2& lhs)					{ return ImVec2( lhs.x,  lhs.y); }
inline ImVec2	operator-(const ImVec2& lhs)					{ return ImVec2(-lhs.x, -lhs.y); }
//inline ImVec2	operator+(const ImVec2& lhs, const ImVec2& rhs)	{ return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
//inline ImVec2	operator-(const ImVec2& lhs, const ImVec2& rhs)	{ return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }
//inline ImVec2	operator*(const ImVec2& lhs, float rhs)			{ return ImVec2(lhs.x * rhs,   lhs.y * rhs); }
inline ImVec2	operator*(float lhs, const ImVec2& rhs)			{ return ImVec2(lhs * rhs.x, lhs * rhs.y); }

enum Integrator
{
	VerletPosition = 0,
	VerletVelocity
};

inline
f32 Dot(ImVec2 a, ImVec2 b)
{
	return a.x*b.x + a.y*b.y;
}

inline
f32 Norm2Sqr(ImVec2 v)
{
	return Dot(v, v);
}

inline
f32 Norm2(ImVec2 v)
{
	return std::sqrtf(Dot(v, v));
}

inline
ImVec2 Normalized(ImVec2 v)
{
	f32 const fInvNorm = 1.0f/Norm2(v);

	return v*fInvNorm;
}

static
void GLFWErrorCallback(int error, const char* description)
{
	std::cerr << "Glfw Error " << error << ": " << description << std::endl;
}

static
ImFont* ImGui_LoadFont(ImFontAtlas& oAtlas, char const* name, f32 const fSize, ImVec2 const& vDisplayOffset = ImVec2(0, 0))
{
	char* windir = nullptr;
	if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
		return nullptr;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		//0x0104, 0x017C, // Polish characters and more
		0x0100, 0x0500, // Polish characters and more
		0,
	};
	ImFontConfig config;
	config.OversampleH = 4;
	config.OversampleV = 4;
	config.PixelSnapH = false;
	//config.MergeMode	= true;

	//auto path = std::string(windir) + "\\Fonts\\" + name;
	//auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);
	ImFont* pFont = oAtlas.AddFontFromFileTTF(name, fSize, &config, ranges);
	if (pFont)
		pFont->DisplayOffset = vDisplayOffset;

	free(windir);

	return pFont;
}

static
void ImGui_AddLoadFont( ImGuiIO& io, char const* name, f32 const fSize, UInt16 const uRangeMin, UInt16 const uRangeMax, ImVec2 const& vDisplayOffset = ImVec2( 0, 0 ) )
{
	char* windir = nullptr;
	if ( _dupenv_s( &windir, nullptr, "WINDIR" ) || windir == nullptr )
		return;

	static const ImWchar ranges[] =
	{
		uRangeMin, uRangeMax, 
		0,
	};

	ImFontConfig config;
	config.OversampleH	= 4;
	config.OversampleV	= 4;
	config.PixelSnapH	= false;
	config.MergeMode	= true;

	//auto path = std::string(windir) + "\\Fonts\\" + name;
	//auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);
	io.Fonts->AddFontFromFileTTF( name, fSize, &config, ranges );
}

void	InitializeImGUIStyle()
{
	ImGuiStyle& oStyle = ImGui::GetStyle();
	oStyle.TabRounding			= 0.0f;
	oStyle.FrameBorderSize		= 1.0f;
	oStyle.ScrollbarRounding	= 0.0f;
	oStyle.ScrollbarSize		= 10.0f;
	ImVec4* pColors = ImGui::GetStyle().Colors;
	pColors[ ImGuiCol_Text					]	= ImVec4( 0.95f, 0.95f, 0.95f, 1.00f );
	pColors[ ImGuiCol_TextDisabled			]	= ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
	pColors[ ImGuiCol_WindowBg				]	= ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
	pColors[ ImGuiCol_ChildBg				]	= ImVec4( 0.04f, 0.04f, 0.04f, 0.50f );
	pColors[ ImGuiCol_PopupBg				]	= ImVec4( 0.12f, 0.12f, 0.12f, 0.94f );
	pColors[ ImGuiCol_Border				]	= ImVec4( 0.25f, 0.25f, 0.27f, 0.50f );
	pColors[ ImGuiCol_BorderShadow			]	= ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	pColors[ ImGuiCol_FrameBg				]	= ImVec4( 0.20f, 0.20f, 0.22f, 0.50f );
	pColors[ ImGuiCol_FrameBgHovered		]	= ImVec4( 0.25f, 0.25f, 0.27f, 0.75f );
	pColors[ ImGuiCol_FrameBgActive			]	= ImVec4( 0.30f, 0.30f, 0.33f, 1.00f );
	pColors[ ImGuiCol_TitleBg				]	= ImVec4( 0.04f, 0.04f, 0.04f, 1.00f );
	pColors[ ImGuiCol_TitleBgActive			]	= ImVec4( 0.04f, 0.04f, 0.04f, 1.00f );
	pColors[ ImGuiCol_TitleBgCollapsed		]	= ImVec4( 0.04f, 0.04f, 0.04f, 0.75f );
	pColors[ ImGuiCol_MenuBarBg				]	= ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	pColors[ ImGuiCol_ScrollbarBg			]	= ImVec4( 0.24f, 0.24f, 0.26f, 0.75f );
	pColors[ ImGuiCol_ScrollbarGrab			]	= ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	pColors[ ImGuiCol_ScrollbarGrabHovered	]	= ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	pColors[ ImGuiCol_ScrollbarGrabActive	]	= ImVec4( 0.94f, 0.92f, 0.94f, 0.75f );
	pColors[ ImGuiCol_CheckMark				]	= ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
	pColors[ ImGuiCol_SliderGrab			]	= ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	pColors[ ImGuiCol_SliderGrabActive		]	= ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	pColors[ ImGuiCol_Button				]	= ImVec4( 0.20f, 0.20f, 0.22f, 1.00f );
	pColors[ ImGuiCol_ButtonHovered			]	= ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	pColors[ ImGuiCol_ButtonActive			]	= ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	pColors[ ImGuiCol_Header				]	= ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	pColors[ ImGuiCol_HeaderHovered			]	= ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	pColors[ ImGuiCol_HeaderActive			]	= ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	pColors[ ImGuiCol_Separator				]	= ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	pColors[ ImGuiCol_SeparatorHovered		]	= ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	pColors[ ImGuiCol_SeparatorActive		]	= ImVec4( 0.62f, 0.62f, 0.62f, 1.00f );
	pColors[ ImGuiCol_ResizeGrip			]	= ImVec4( 0.30f, 0.30f, 0.33f, 0.75f );
	pColors[ ImGuiCol_ResizeGripHovered		]	= ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	pColors[ ImGuiCol_ResizeGripActive		]	= ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	pColors[ ImGuiCol_Tab					]	= ImVec4( 0.21f, 0.21f, 0.22f, 1.00f );
	pColors[ ImGuiCol_TabHovered			]	= ImVec4( 0.37f, 0.37f, 0.39f, 1.00f );
	pColors[ ImGuiCol_TabActive				]	= ImVec4( 0.30f, 0.30f, 0.33f, 1.00f );
	pColors[ ImGuiCol_TabUnfocused			]	= ImVec4( 0.12f, 0.12f, 0.12f, 0.97f );
	pColors[ ImGuiCol_TabUnfocusedActive	]	= ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	pColors[ ImGuiCol_DockingPreview		]	= ImVec4( 0.26f, 0.59f, 0.98f, 0.50f );
	pColors[ ImGuiCol_DockingEmptyBg		]	= ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
	pColors[ ImGuiCol_PlotLines				]	= ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
	pColors[ ImGuiCol_PlotLinesHovered		]	= ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
	pColors[ ImGuiCol_PlotHistogram			]	= ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
	pColors[ ImGuiCol_PlotHistogramHovered	]	= ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
	pColors[ ImGuiCol_TextSelectedBg		]	= ImVec4( 0.26f, 0.59f, 0.98f, 0.50f );
	pColors[ ImGuiCol_DragDropTarget		]	= ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
	pColors[ ImGuiCol_NavHighlight			]	= ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
	pColors[ ImGuiCol_NavWindowingHighlight	]	= ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	pColors[ ImGuiCol_NavWindowingDimBg		]	= ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
	pColors[ ImGuiCol_ModalWindowDimBg		]	= ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
	oStyle.WindowMenuButtonPosition = ImGuiDir_Right;

	ImGuiIO& io = ImGui::GetIO();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	//ImGuiStyle& style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		oStyle.WindowRounding = 0.0f;
		oStyle.Colors[ ImGuiCol_WindowBg ].w = 1.0f;
	}
}

std::vector< ImVec2 >	oPendulumPos;
std::vector< ImVec2 >	oPendulumPosPrev;
std::vector< ImVec2 >	oPendulumVel;
std::vector< ImVec2 >	oPendulumVelPrev;
std::vector< ImVec2 >	oPendulumAcc;
std::vector< ImVec2 >	oPendulumAccPrev;
std::vector< f32 >		oPendulumMass;

f32						fG			= 9.81f;
f32						fDt			= 0.0125f;
Int32					iIter		= 32;
Int32					iLinksCount	= 5;
Bool					bSimulate	= false;

void	Constraints()
{
	for ( Int32 iIdx = 0; iIdx < iIter; ++iIdx )
	{
		for ( Int32 iLinkIdx = 1; iLinkIdx <= iLinksCount; ++iLinkIdx )
		{
			ImVec2& vA = oPendulumPos[ iLinkIdx - 1 ];
			ImVec2& vB = oPendulumPos[ iLinkIdx - 0 ];

			ImVec2	const	vDelta	= vB - vA;
			f32		const	fNorm2	= Norm2( vDelta );
			f32		const	fDelta	= fNorm2 - 1.0f;
			ImVec2	const	vDir	= Normalized( vDelta );

			vA = vA + vDir*fDelta*0.5f;
			vB = vB - vDir*fDelta*0.5f;
		}
		oPendulumPos[ 0 ] = ImVec2( 0, 0 );
	}
}

void	SimulateStormerVerlet()
{
	auto oItPrev	= oPendulumPosPrev.begin();
	auto oItAcc		= oPendulumAcc.begin();
	auto oItMass	= oPendulumMass.begin();

	f32 const fDt2 = fDt*fDt;

	for ( ImVec2& v : oPendulumPos )
	{
		f32 const m		= *oItMass;
		f32 const ax	= m > 0.0f ? oItAcc->x/m : 0.0f;
		f32 const ay	= m > 0.0f ? oItAcc->y/m : 0.0f;
		// Verlet Integration
		v.x = 2.0f*v.x - oItPrev->x + ax*fDt2;
		v.y = 2.0f*v.y - oItPrev->y + ay*fDt2;

		++oItPrev;
		++oItAcc;
		++oItMass;
	}

	Constraints();
}

void	SimulateLeapfrog()
{
	auto oItPrev	= oPendulumPosPrev.begin();
	auto oItVel		= oPendulumVel.begin();
	auto oItAcc		= oPendulumAcc.begin();
	auto oItMass	= oPendulumMass.begin();

	f32 const fDt2 = fDt*fDt;

	for ( ImVec2& v : oPendulumPos )
	{
		f32 const m		= *oItMass;
		f32 const ax	= m > 0.0f ? oItAcc->x/m : 0.0f;
		f32 const ay	= m > 0.0f ? oItAcc->y/m : 0.0f;

		v.x += oItVel->x*fDt + ax*fDt2*0.5f;
		v.y += oItVel->y*fDt + ay*fDt2*0.5f;

		oItVel->x += ax*fDt;
		oItVel->y += ay*fDt;

		++oItPrev;
		++oItVel;
		++oItAcc;
		++oItMass;
	}

	Constraints();
}

Int32 iSelectedIntegrator = 0;
typedef void ( *pfSimPtr )();
std::vector< pfSimPtr >		oIntegrator			= { &SimulateStormerVerlet, &SimulateLeapfrog };
std::vector< std::string >	oIntegratorNames	= { "Stormer-Verlet (Position Verlet)", "Leapfrog (Velocity Verlet)" };
pfSimPtr					pfCurrentIntegrator = &SimulateStormerVerlet;

void	Simulate()
{
	std::vector< ImVec2 > oPosCopy = oPendulumPos;
	std::vector< ImVec2 > oVelCopy = oPendulumVel;
	std::vector< ImVec2 > oAccCopy = oPendulumAcc;

	( *pfCurrentIntegrator )();

	oPendulumPosPrev = oPosCopy;
	oPendulumVelPrev = oVelCopy;
	oPendulumAccPrev = oAccCopy;
}

void	Reset()
{
	oPendulumPos.clear();
	oPendulumPos.reserve( iLinksCount + 1 );

	oPendulumVel.clear();
	oPendulumVel.reserve( iLinksCount + 1 );

	oPendulumAcc.clear();
	oPendulumAcc.reserve( iLinksCount + 1 );

	oPendulumMass.clear();
	oPendulumMass.reserve( iLinksCount + 1 );

	oPendulumPos.emplace_back( 0.0f, 0.0f );
	oPendulumVel.emplace_back( 0.0f, 0.0f );

	oPendulumMass.push_back( 0.0f );

	f32 fPosX = 0.0f;
	f32 fPosY = 0.0f;
	for ( Int32 iIdx = 0; iIdx < iLinksCount; ++iIdx )
	{
		fPosX += 1.0f; // Link length
		fPosY += 0.0f;

		oPendulumPos.emplace_back( fPosX, fPosY );
		oPendulumVel.emplace_back( 0.0f, 0.0f );
		oPendulumMass.push_back( f32( iLinksCount - iIdx ) );
	}

	oPendulumAcc.resize( iLinksCount + 1 );
	std::fill( oPendulumAcc.begin(), oPendulumAcc.end(), ImVec2( 0.0f, fG ) );
	oPendulumAcc[ 0 ] = ImVec2( 0.0f, 0.0f );

	oPendulumPosPrev = oPendulumPos;
	oPendulumVelPrev = oPendulumVel;
	oPendulumAccPrev = oPendulumAcc;
}

int main()
{
	glfwSetErrorCallback( GLFWErrorCallback );

	if ( !glfwInit() )
		return 1;

	char const* glsl_version = "#version 430";
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	//glfwWindowHint( GLFW_VISIBLE, false );

	GLFWwindow* pWindow = glfwCreateWindow( 1280, 720, "NPendulum", nullptr, nullptr );
	if ( pWindow == nullptr )
		return 1;

	glfwMakeContextCurrent( pWindow );
	//glfwSwapInterval( 1 ); // Enable vsync

	Bool bErr = gladLoadGL() == 0;
	if ( bErr )
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;

		return 1;
	}

	IMGUI_CHECKVERSION();

	ImFontAtlas oFontAtlas;
	auto defaultFont = ImGui_LoadFont( oFontAtlas, "./CONSOLA.TTF", 22.0f );
	oFontAtlas.Build();
	ImGui::CreateContext( &oFontAtlas );

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	InitializeImGUIStyle();

	ImGui_ImplGlfw_InitForOpenGL( pWindow, true );
	ImGui_ImplOpenGL3_Init( glsl_version );

	ImVec4 const vOrange	= ImVec4(255.f/255.f,	128.f/255.f,  0.f,			1.0f);
	ImVec4 const vBlue		= ImVec4(  0.f/255.f,	128.f/255.f, 255.f/255.f,	1.0f);
	ImVec4 const vGreen		= ImVec4( 64.f/255.f,	192.f/255.f,  64.f/255.f,	1.0f);
	ImVec4 const vPurple	= ImVec4(1.f - 64.f/255.f, 1.0f - 192.f/255.f, 1.0f - 64.f/255.f, 1.0f);

	Reset();

	ImGuiDockNodeFlags	eDockspaceFlags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags	eWindowFlags	=	ImGuiWindowFlags_MenuBar |
											ImGuiWindowFlags_NoDocking |
											ImGuiWindowFlags_AlwaysAutoResize;

	eWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	eWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImVec4 vClearColor = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
	while ( !glfwWindowShouldClose( pWindow ) )
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos		( viewport->Pos );
			ImGui::SetNextWindowSize	( viewport->Size );
			ImGui::SetNextWindowViewport( viewport->ID );

			Bool bOpen = true;
			ImGui::Begin( "Dockspace", &bOpen, eWindowFlags );

			ImGuiID uDockspaceID = ImGui::GetID( "_DockSpace" );
			ImGui::DockSpace( uDockspaceID, ImVec2( 0.0f, 0.0f ), eDockspaceFlags );

			ImGui::Begin( "Infos" );
				if ( ImGui::SliderInt( "Links Count", &iLinksCount, 1, 15 ) )
				{
					Reset();
				}
				if ( ImGui::Button( bSimulate ? "Stop" : "Simulate" ) )
				{
					bSimulate = !bSimulate;
				}
				if ( ImGui::Button( "Reset" ) )
				{
					Reset();
				}
				ImGui::InputFloat( "Gravity", &fG, 1.0f, 0.0f );
				ImGui::InputFloat( ( char const* )u8"\u0394t", &fDt, 1.0f, 0.0f );
				if ( ImGui::InputInt( "Iteration", &iIter ) )
				{
					iIter = std::max( iIter, 1 );
				}
				std::string sItem = oIntegratorNames[ iSelectedIntegrator ];
				if ( ImGui::BeginCombo( "Integrator", sItem.c_str(), 0 ) )
				{
					Int32 iIdx = 0;
					Int32 iNewIdx = -1;
					for ( std::string const& sIntegrator : oIntegratorNames )
					{
						Bool bIsSelected = ( sItem == sIntegrator );
						if ( ImGui::Selectable( sIntegrator.c_str(), bIsSelected ) )
						{
							sItem = sIntegrator;
							iNewIdx = iIdx;
						}
						if ( bIsSelected )
							ImGui::SetItemDefaultFocus();
						++iIdx;
					}
					ImGui::EndCombo();
					if ( iNewIdx >= 0)
					{
						pfCurrentIntegrator = oIntegrator[ iNewIdx ];
						iSelectedIntegrator = iNewIdx;
					}
				}
			ImGui::End();

			ImGui::Begin( "Draw" );
				ImDrawList*	pDrawList	= ImGui::GetWindowDrawList();
				ImVec2		vPos		= ImGui::GetCursorScreenPos();
				ImVec2		vSize		= ImGui::GetWindowContentRegionMax();
				ImVec2		vCenter		= ImVec2( vPos.x + vSize.x*0.5f, vPos.y + vSize.y*0.5f );

				constexpr f32 fSecurityZone	= 0.05f; // 10%
				f32 const fMinSize	= std::min( vSize.x, vSize.y )*( 1.0f - fSecurityZone );
				f32 const fLinkSize	= 0.5f*fMinSize/f32( iLinksCount + 1 );

				std::vector< ImVec2 > oVizPendulum;

				oVizPendulum.clear();
				oVizPendulum.reserve( iLinksCount + 1 );

				f32 fPosX = vPos.x + vSize.x*0.5f;
				f32 fPosY = vPos.y + vSize.y*0.5f;

				oVizPendulum.emplace_back( fPosX, fPosY );

				for ( Int32 iIdx = 0; iIdx < iLinksCount; ++iIdx )
				{
					fPosX = vCenter.x + oPendulumPos[ iIdx ].x*fLinkSize;
					fPosY = vCenter.y + oPendulumPos[ iIdx ].y*fLinkSize;

					oVizPendulum.emplace_back( fPosX, fPosY );
				}
				pDrawList->AddPolyline( oVizPendulum.data(), int( oVizPendulum.size() ), ImGui::GetColorU32( vBlue ), false, 8.0f );

				for ( Int32 iIdx = 1; iIdx <= iLinksCount; ++iIdx )
				{
					f32 const m = oPendulumMass[ iIdx ];
					f32 const r = 8.0f*m/f32( iLinksCount );

					pDrawList->AddCircleFilled( oVizPendulum[ iIdx ], r, ImGui::GetColorU32( vOrange ), 16 );
				}
			ImGui::End();

			ImGui::End();

			if ( bSimulate )
			{
				Simulate();
			}
		}

		ImGui::Render();
		Int32 iDisplayW, iDisplayH;
		glfwGetFramebufferSize( pWindow, &iDisplayW, &iDisplayH );
		glViewport( 0, 0, iDisplayW, iDisplayH );
		ImVec4 vClearColor = ImColor( 32, 32, 32, 255 );
		glClearColor( vClearColor.x, vClearColor.y, vClearColor.z, vClearColor.w );
		glClear( GL_COLOR_BUFFER_BIT );
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			GLFWwindow* pBackupCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent( pBackupCurrentContext );
		}

		glfwSwapBuffers( pWindow );
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	glfwDestroyWindow( pWindow );
	glfwTerminate();

	return 0;
}
