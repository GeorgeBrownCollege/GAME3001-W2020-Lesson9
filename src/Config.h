#pragma once
#ifndef __CONFIG__
#define __CONFIG__

class Config {
public:	
	static const int SCREEN_WIDTH = 800;
	static const int SCREEN_HEIGHT = 600;
	static const int ROW_NUM = 15;
	static const int COL_NUM = 20;
	static const int TILE_SIZE = 40;
	static const int TILE_COST = 1;

	template<typename Creator, typename Destructor, typename... Arguments>
	static auto make_resource(Creator c, Destructor d, Arguments&&... args)
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::system_error(errno, std::generic_category()); }
		return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
	}

};

#endif /* defined (__CONFIG__) */