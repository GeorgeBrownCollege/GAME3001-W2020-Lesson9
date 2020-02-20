#include "PlayScene.h"
#include "Game.h"
#include <ctime>
#include "GLM/gtx/string_cast.hpp"
#include <algorithm>
#include "TileComparators.h"
#include <iomanip>
#include "Util.h"


// Pathfinding & Steering functions ***********************************************

void PlayScene::m_resetGrid()
{
	for (auto tile : m_pGrid)
	{
		tile->setTileState(UNDEFINED);
	}
}

void PlayScene::m_buildGrid()
{
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	
	m_pGrid = std::vector<Tile*>(); // instantiates a structure of type vector<Tile*>

	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			auto tile = new Tile(glm::vec2(offset + size * col, offset + size * row), 
				glm::vec2(col, row));
			addChild(tile);
			tile->setTileState(UNDEFINED);
			m_pGrid.push_back(tile);
		}
	}
}

void PlayScene::m_mapTiles()
{
	for (auto tile : m_pGrid)
	{
		float x = tile->getGridPosition().x;
		float y = tile->getGridPosition().y;

		if(y != 0)                   { tile->setUp   (m_pGrid[x + ((y - 1) * Config::COL_NUM)]); }
		if(x != Config::COL_NUM - 1) { tile->setRight(m_pGrid[(x + 1) + (y * Config::COL_NUM)]); }
		if(y != Config::ROW_NUM - 1) { tile->setDown (m_pGrid[x + ((y + 1) * Config::COL_NUM)]); }
		if(x != 0)					 { tile->setLeft (m_pGrid[(x - 1) + (y * Config::COL_NUM)]); }
	}
}

int PlayScene::m_spawnObject(PathFindingDisplayObject* object)
{
	m_resetGrid();
	
	Tile* randomTile = nullptr;
	int randomTileIndex = 0;
	do
	{
		randomTileIndex = int(Util::RandomRange(0, m_pGrid.size() - 1));
		randomTile = m_pGrid[randomTileIndex];
	} while (randomTile->getTileState() != UNDEFINED); // search for empty tile


	if (object->getTile() != nullptr)
	{
		object->getTile()->setTileState(UNDEFINED);
	}

	object->setPosition(randomTile->getPosition());
	object->setTile(randomTile);

	return randomTileIndex;
}

void PlayScene::m_spawnShip()
{
	const auto randomTileIndex = m_spawnObject(m_pShip);
	m_pGrid[randomTileIndex]->setTileState(START);
}

void PlayScene::m_spawnPlanet()
{
	const auto randomTileIndex = m_spawnObject(m_pPlanet);
	m_pGrid[randomTileIndex]->setTileState(GOAL);
	m_computeTileValues();
}

void PlayScene::m_computeTileValues()
{
	for (auto tile : m_pGrid)
	{
		tile->setTargetDistance(m_pPlanet->getTile()->getGridPosition());
	}
}

Tile* PlayScene::m_findLowestCostTile(std::vector<Tile*> neighbours)
{
	Tile* minTile = nullptr;
	float min = INFINITY;
	
	// for every tile in the neighbours vector
	for (auto tile : neighbours)
	{
		if(tile != nullptr)
		{
			if (tile->getTileState() == GOAL)
			{
				return tile;
			}

			if (min > tile->getTileValue())
			{
				min = tile->getTileValue();
				/*if (minTile != nullptr)
				{
					minTile->setTileState(CLOSED);
				}*/

				minTile = tile;
				if (tile->getTileState() == UNDEFINED)
				{
					tile->setTileState(OPEN);
				}


			}
			else
			{
				if (tile->getTileState() == UNDEFINED)
				{
					tile->setTileState(CLOSED);
				}
			}
		}
		
	}

	return minTile;
}

void PlayScene::m_findShortestPath()
{
	
	auto tile = m_pShip->getTile();

	while(tile->getTileState() != GOAL)
	{
		const auto neighbours = tile->getNeighbours();

		tile = m_findLowestCostTile(neighbours);
	}
}


// ImGui functions ***********************************************

void PlayScene::m_ImGuiKeyMap()
{
	ImGuiIO& io = ImGui::GetIO();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void PlayScene::m_ImGuiSetStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 0.8f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void PlayScene::m_updateUI()
{
	// Prepare Window Frame
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // use for debug purposes

	std::string windowString = "Settings ";

	ImGui::Begin(&windowString[0], NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

	// set window to top left corner
	ImGui::SetWindowPos(ImVec2(0, 0), true);

	/*************************************************************************************************/
	/* MENU                                                                                          */
	/*************************************************************************************************/

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Separator();
			ImGui::MenuItem("Exit", NULL, &m_exitApp);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &m_displayAbout);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (m_exitApp)
	{
		TheGame::Instance()->quit();
	}

	if (m_displayAbout)
	{
		ImGui::Begin("About Pathfinding Simulator", &m_displayAbout, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Separator();
		ImGui::Text("Authors:");
		ImGui::Text("Tom Tsiliopoulos ");
		ImGui::End();
	}

	/*************************************************************************************************/
	if (ImGui::Button("Respawn Ship"))
	{
		m_spawnShip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Planet"))
	{
		m_spawnPlanet();
	}

	ImGui::SameLine();

	if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	

	ImGui::PushItemWidth(80);
	/*if (ImGui::SliderFloat("Some Float", &myFloat, 0.1f, 10.0f, "%.1f"))
	{
		
	}*/


	
	ImGui::PopItemWidth();

	if (ImGui::CollapsingHeader("Some Collapsing Header"))
	{
		ImGui::PushItemWidth(80);

		ImGui::PopItemWidth();
	}

	

	// Main Window End
	ImGui::End();
}

/*** SCENE FUNCTIONS ***/
void PlayScene::m_resetAll()
{
	
}

void PlayScene::start()
{
	m_buildGrid();
	m_mapTiles();

	// instantiate planet and add it to the DisplayList
	m_pPlanet = new Planet();
	addChild(m_pPlanet);

	m_spawnPlanet();
	
	// instantiate ship and add it to the DisplayList
	m_pShip = new Ship();
	addChild(m_pShip);

	m_spawnShip();
}

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
{
}

void PlayScene::draw()
{
	if (m_displayUI)
	{
		for (auto tile : m_pGrid)
		{
			tile->draw();
		}
	}

	m_pPlanet->draw();
	
	m_pShip->draw();
	

	// ImGui Rendering section - DO NOT MOVE OR DELETE
	if (m_displayUI)
	{
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 255, 255, 255, 255);
	}
}

void PlayScene::update()
{
	/*m_pTile->update();
	m_pShip->update();*/

	if (m_displayUI)
	{
		m_updateUI();
	}

}

void PlayScene::clean()
{
	m_pShip->clean();

	removeAllChildren();
}

void PlayScene::handleEvents()
{
	ImGuiIO& io = ImGui::GetIO();
	int wheel = 0;

	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			wheel = event.wheel.y;
			break;
		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(event.text.text);
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;
			case SDLK_BACKQUOTE:
				m_displayUI = (m_displayUI) ? false : true;
				break;
			case SDLK_f:
				m_findShortestPath();
				break;
			case SDLK_g:
				
				break;
			case SDLK_m:
				m_spawnShip();
				break;
			case SDLK_p:
				m_spawnPlanet();
				break;
			case SDLK_r:
				m_resetAll();
				break;

				/************************************************************************/
			case SDLK_w:
				
				break;
			case SDLK_s:
				
				break;
			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			}
			{
				int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				
				break;

			case SDLK_s:
				
				break;

			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			}
			{
				int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		default:
			break;
		}
	}

	io.DeltaTime = 1.0f / 60.0f;
	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	io.DisplaySize.x = 1280;
	io.DisplaySize.y = 720;

	m_ImGuiKeyMap();
	m_ImGuiSetStyle();
}
