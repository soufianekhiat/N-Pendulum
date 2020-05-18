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
		0x0104, 0x017C, // Polish characters and more
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
	glfwSwapInterval( 1 ); // Enable vsync

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

	ImVec4 vClearColor = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );

	while ( !glfwWindowShouldClose( pWindow ) )
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGuiDockNodeFlags	dockspace_flags = ImGuiDockNodeFlags_None;
			ImGuiWindowFlags	eWindowFlags	=	ImGuiWindowFlags_MenuBar |
													ImGuiWindowFlags_NoDocking |
													ImGuiWindowFlags_AlwaysAutoResize;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos		( viewport->Pos );
			ImGui::SetNextWindowSize	( viewport->Size );
			ImGui::SetNextWindowViewport( viewport->ID );

			eWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			eWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			Bool bOpen = true;
			ImGui::Begin( "Dockspace", &bOpen, eWindowFlags );

			ImGuiID uDockspaceID = ImGui::GetID( "_DockSpace" );
			ImGui::DockSpace( uDockspaceID, ImVec2( 0.0f, 0.0f ), dockspace_flags );

			ImGui::Begin( "Infos" );
			ImGui::End();

			ImGui::Begin( "Draw" );
			ImGui::End();

			ImGui::End();
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
