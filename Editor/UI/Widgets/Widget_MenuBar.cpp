/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ========================
#include "Widget_MenuBar.h"
#include "../../ImGui/Source/imgui.h"
#include "../FileDialog.h"
#include "Core/Engine.h"
#include "Core/Settings.h"
#include "Profiling/Profiler.h"
#include "Rendering/Renderer.h"
//===================================

//= NAMESPACES ==========
using namespace std;
using namespace Directus;
//=======================

static bool g_showAboutWindow		= false;
static bool g_showMetricsWindow		= false;
static bool g_showStyleEditor		= false;
static bool g_fileDialogVisible		= false;
static bool g_showResourceCache		= false;
static bool g_showProfiler			= false;
static bool g_showRendererOptions	= false;
static string g_fileDialogSelection;
ResourceManager* g_resourceManager	= nullptr;
Scene* g_scene						= nullptr;

const char* g_rendererViews[] =
{
	"Default",
	"Albedo",
	"Normal",
	"Specular",
	"Depth"
};
static int g_rendererViewInt = 0;
static const char* g_rendererView = g_rendererViews[g_rendererViewInt];

Widget_MenuBar::Widget_MenuBar()
{
	m_isWindow = false;
}

void Widget_MenuBar::Initialize(Context* context)
{
	Widget::Initialize(context);
	g_resourceManager = m_context->GetSubsystem<ResourceManager>();
	g_scene = m_context->GetSubsystem<Scene>();
	m_fileDialog = make_unique<FileDialog>(m_context, true, FileDialog_Scene);
}

void Widget_MenuBar::Update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("New"))
			{
				m_context->GetSubsystem<Scene>()->Clear();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Load"))
			{
				m_fileDialog->SetStyle(FileDialog_Load);
				g_fileDialogVisible = true;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{
				m_fileDialog->SetStyle(FileDialog_Save);
				g_fileDialogVisible = true;
			}

			if (ImGui::MenuItem("Save As..."))
			{
				m_fileDialog->SetStyle(FileDialog_Save);
				g_fileDialogVisible = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", nullptr, &g_showMetricsWindow);
			ImGui::MenuItem("Style", nullptr, &g_showStyleEditor);
			ImGui::MenuItem("Resource Cache Viewer", nullptr, &g_showResourceCache);
			ImGui::MenuItem("Profiler", nullptr, &g_showProfiler);
			ImGui::MenuItem("Renderer Options", nullptr, &g_showRendererOptions);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", nullptr, &g_showAboutWindow);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (g_showMetricsWindow)	ImGui::ShowMetricsWindow();
	if (g_showStyleEditor)		ImGui::ShowStyleEditor();
	if (g_fileDialogVisible)	ShowFileDialog();
	if (g_showAboutWindow)		ShowAboutWindow();
	if (g_showResourceCache)	ShowResourceCache();
	if (g_showProfiler)			ShowProfiler();
	if (g_showRendererOptions)	ShowRendererOptions();
}

void Widget_MenuBar::ShowFileDialog()
{
	if (!m_fileDialog->Show(&g_fileDialogVisible, &g_fileDialogSelection))
		return;

	// LOAD
	if (m_fileDialog->GetStyle() == FileDialog_Open || m_fileDialog->GetStyle() == FileDialog_Load)
	{
		// Scene
		if (FileSystem::IsEngineSceneFile(g_fileDialogSelection))
		{
			EditorHelper::Get().LoadScene(g_fileDialogSelection);
			g_fileDialogVisible = false;
		}
	}
	// SAVE
	else if (m_fileDialog->GetStyle() == FileDialog_Save)
	{
		// Scene
		if (m_fileDialog->GetFilter() == FileDialog_Scene)
		{
			EditorHelper::Get().SaveScene(g_fileDialogSelection);
			g_fileDialogVisible = false;
		}
	}
}

void Widget_MenuBar::ShowAboutWindow()
{
	ImGui::Begin("About", &g_showAboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Directus3D %s", ENGINE_VERSION);
	ImGui::Text("Author: Panos Karabelas");
	ImGui::SameLine(600); ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);  if (ImGui::Button("GitHub"))
	{
		FileSystem::OpenDirectoryWindow("https://github.com/PanosK92/Directus3D");
	}	

	ImGui::Separator();

	ImGui::Text("MIT License");
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files(the \"Software\"), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and / or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is furnished");
	ImGui::Text("to do so, subject to the following conditions :");
	ImGui::Text("The above copyright notice and this permission notice shall be included in");
	ImGui::Text("all copies or substantial portions of the Software.");
	ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS");
	ImGui::Text("FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR");
	ImGui::Text("COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER");
	ImGui::Text("IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN");
	ImGui::Text("CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::Separator();

	ImGui::Text("Third party libraries");

	static float columnA = 120;
	static float columnB = 200;

	ImGui::BulletText("AngelScript");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionAngelScript).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_AngelScript"); if (ImGui::Button("Website")) { FileSystem::OpenDirectoryWindow("https://www.angelcode.com/angelscript/"); } ImGui::PopID();

	ImGui::BulletText("Assimp");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionAssimp).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_Assimp"); if (ImGui::Button("GitHub")) { FileSystem::OpenDirectoryWindow("https://github.com/assimp/assimp"); } ImGui::PopID();

	ImGui::BulletText("Bullet");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionBullet).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_Bullet"); if (ImGui::Button("GitHub")) { FileSystem::OpenDirectoryWindow("https://github.com/bulletphysics/bullet3"); } ImGui::PopID();

	ImGui::BulletText("FMOD");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionFMOD).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_FMOD"); if (ImGui::Button("Website")) { FileSystem::OpenDirectoryWindow("https://www.fmod.com/"); } ImGui::PopID();

	ImGui::BulletText("FreeImage");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionFreeImage).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_FreeImage"); if (ImGui::Button("SourceForge")) { FileSystem::OpenDirectoryWindow("https://sourceforge.net/projects/freeimage/files/Source%20Distribution/"); } ImGui::PopID();

	ImGui::BulletText("FreeType");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionFreeType).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_FreeType"); if (ImGui::Button("SourceForge")) { FileSystem::OpenDirectoryWindow("https://sourceforge.net/projects/freetype/files/freetype2/"); } ImGui::PopID();

	ImGui::BulletText("ImGui");	
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionImGui).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_ImGui"); if (ImGui::Button("GitHub")) { FileSystem::OpenDirectoryWindow("https://github.com/ocornut/imgui"); } ImGui::PopID();

	ImGui::BulletText("PugiXML");
	ImGui::SameLine(columnA); ImGui::Text(("v" + Settings::Get().m_versionPugiXML).c_str()); ImGui::SameLine(columnB);
	ImGui::PushID("Button_PugiXML");  if (ImGui::Button("GitHub")) { FileSystem::OpenDirectoryWindow("https://github.com/zeux/pugixml"); } ImGui::PopID();

	ImGui::End();
}

void Widget_MenuBar::ShowResourceCache()
{
	auto resources = m_context->GetSubsystem<ResourceManager>()->GetResourceAll();
	auto totalMemoryUsage =  m_context->GetSubsystem<ResourceManager>()->GetMemoryUsage() / 1000.0f / 1000.0f;

	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Resource Cache Viewer", &g_showResourceCache, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::Text("Resource count: %d, Total memory usage: %d Mb", (int)resources.size(), (int)totalMemoryUsage);
	ImGui::Separator();
	ImGui::Columns(5, "##MenuBar::ShowResourceCacheColumns");
	ImGui::Text("Type"); ImGui::NextColumn();
	ImGui::Text("ID"); ImGui::NextColumn();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Path"); ImGui::NextColumn();
	ImGui::Text("Size"); ImGui::NextColumn();
	ImGui::Separator();
	for (const auto& resource : resources)
	{
		if (!resource)
			continue;

		// Type
		ImGui::Text(resource->GetResourceType_cstr());					ImGui::NextColumn();

		// ID
		ImGui::Text(to_string(resource->GetResourceID()).c_str());		ImGui::NextColumn();

		// Name
		ImGui::Text(resource->GetResourceName().c_str());				ImGui::NextColumn();

		// Path
		ImGui::Text(resource->GetResourceFilePath().c_str());			ImGui::NextColumn();

		// Memory
		auto memory = (unsigned int)(resource->GetMemory() / 1000.0f); // default in Kb
		if (memory <= 1024)
		{
			ImGui::Text((to_string(memory) + string(" Kb")).c_str());	ImGui::NextColumn();
		}
		else
		{
			memory = (unsigned int)(memory / 1000.0f); // turn into Mb
			ImGui::Text((to_string(memory) + string(" Mb")).c_str());	ImGui::NextColumn();
		}		
	}
	ImGui::Columns(1);

	ImGui::End();
}

void Widget_MenuBar::ShowProfiler()
{
	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", &g_showProfiler, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::Columns(2, "##MenuBar::ShowProfilerColumns");
	ImGui::Text("Function"); ImGui::NextColumn();
	ImGui::Text("Duration"); ImGui::NextColumn();
	ImGui::Separator();

	for (const auto& block : Profiler::Get().GetAllBlocks())
	{
		ImGui::Text("%s", block.first); ImGui::NextColumn();
		ImGui::Text("%f ms", block.second.duration); ImGui::NextColumn();	
	}
	ImGui::Columns(1);

	ImGui::End();
}

void Widget_MenuBar::ShowRendererOptions()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
	ImGui::Begin("Renderer Options", &g_showRendererOptions, ImGuiWindowFlags_NoResize);
	float posX = 100.0f;

	bool bloom		= Renderer:: RenderFlags_IsSet(Render_Bloom);
	bool fxaa		= Renderer:: RenderFlags_IsSet(Render_FXAA);
	bool sharpening	= Renderer:: RenderFlags_IsSet(Render_Sharpening);

	// Bloom
	ImGui::Text("Bloom");
	ImGui::SameLine(posX); ImGui::Checkbox("##rendererBloom", &bloom);

	// FXAA
	ImGui::Text("FXAA");
	ImGui::SameLine(posX); ImGui::Checkbox("##rendererFXAA", &fxaa);

	// FXAA
	ImGui::Text("Sharpening");
	ImGui::SameLine(posX); ImGui::Checkbox("##rendererSharpening", &sharpening);

	// G-Buffer Visualization
	ImGui::Text("G-Buffer");
	ImGui::SameLine(posX); if (ImGui::BeginCombo("##rendererGBuffer", g_rendererView))
	{
		for (int i = 0; i < IM_ARRAYSIZE(g_rendererViews); i++)
		{
			bool is_selected = (g_rendererView == g_rendererViews[i]);
			if (ImGui::Selectable(g_rendererViews[i], is_selected))
			{
				g_rendererView = g_rendererViews[i];
				g_rendererViewInt = i;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	bloom		? Renderer:: RenderFlags_Enable(Render_Bloom)		: Renderer:: RenderFlags_Disable(Render_Bloom);
	fxaa		? Renderer:: RenderFlags_Enable(Render_FXAA)		: Renderer:: RenderFlags_Disable(Render_FXAA);
	sharpening	? Renderer:: RenderFlags_Enable(Render_Sharpening)	: Renderer:: RenderFlags_Disable(Render_Sharpening);

	// G-buffer
	if (g_rendererViewInt == 0) // Combined
	{
		Renderer::RenderFlags_Disable(Render_Albedo);
		Renderer::RenderFlags_Disable(Render_Normal);
		Renderer::RenderFlags_Disable(Render_Specular);
		Renderer::RenderFlags_Disable(Render_Depth);
	}
	else if (g_rendererViewInt == 1) // Albedo
	{
		Renderer::RenderFlags_Enable(Render_Albedo);
		Renderer::RenderFlags_Disable(Render_Normal);
		Renderer::RenderFlags_Disable(Render_Specular);
		Renderer::RenderFlags_Disable(Render_Depth);
	}
	else if (g_rendererViewInt == 2) // Normal
	{
		Renderer::RenderFlags_Disable(Render_Albedo);
		Renderer::RenderFlags_Enable(Render_Normal);
		Renderer::RenderFlags_Disable(Render_Specular);
		Renderer::RenderFlags_Disable(Render_Depth);
	}
	else if (g_rendererViewInt == 3) // Specular
	{
		Renderer::RenderFlags_Disable(Render_Albedo);
		Renderer::RenderFlags_Disable(Render_Normal);
		Renderer::RenderFlags_Enable(Render_Specular);
		Renderer::RenderFlags_Disable(Render_Depth);
	}
	else if (g_rendererViewInt == 4) // Depth
	{
		Renderer::RenderFlags_Disable(Render_Albedo);
		Renderer::RenderFlags_Disable(Render_Normal);
		Renderer::RenderFlags_Disable(Render_Specular);
		Renderer::RenderFlags_Enable(Render_Depth);
	}

	ImGui::End();
}