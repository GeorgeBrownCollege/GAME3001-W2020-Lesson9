#include "FontManager.h"
#include "Game.h"

FontManager* FontManager::s_pInstance = nullptr;

inline bool FontManager::m_exists(const std::string& id)
{
	return m_fontMap.find(id) != m_fontMap.end();
}

bool FontManager::load(const std::string& file_name, const std::string& id, const int size, const int style)
{
	if(m_exists(id))
	{
		return true;
	}
	
	const auto font = TTF_OpenFont(file_name.c_str(), size);
	if (font != nullptr)
	{
		TTF_SetFontStyle(font, style);
		m_fontMap[id] = font;
		return true;
	}

	return false;
}

bool FontManager::textToTexture(const std::string& text, const std::string& font_id, const std::string& texture_id, const SDL_Color colour)
{
	//Render text surface

	const auto textSurface = TTF_RenderText_Solid(m_fontMap[font_id] , text.c_str(), colour);
	if (textSurface == nullptr)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	else
	{
		//Create texture from surface pixels
		TextureManager::Instance()->addTexture(texture_id, SDL_CreateTextureFromSurface(TheGame::Instance()->getRenderer(), textSurface));
		
		if (TextureManager::Instance()->getTexture(texture_id) == nullptr)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			return false;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//TTF_CloseFont(m_fontMap[font_id]);
	//m_fontMap.erase(font_id);

	return true;
}

TTF_Font * FontManager::getFont(const std::string& id)
{
	return m_fontMap[id];
}

void FontManager::clean()
{
	auto it = m_fontMap.begin();
	while (it != m_fontMap.end())
	{
		TTF_CloseFont(it->second);
		it->second = nullptr;
		m_fontMap.erase(it++);
	}

	m_fontMap.clear();
}

void FontManager::displayFontMap()
{
	std::cout << "------------ Displaying Font Map -----------" << std::endl;

	std::cout << "Font Map size: " << m_fontMap.size() << std::endl;

	auto it = m_fontMap.begin();
	while (it != m_fontMap.end())
	{
		std::cout << " " << it->first << std::endl;
		++it;
	}
}


FontManager::FontManager()
= default;

FontManager::~FontManager()
= default;
