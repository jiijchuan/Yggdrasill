#pragma once
#include "global.h"

const char FENCE = '+';
const char SPACE = ' ';

typedef std::vector<std::vector<char>> snake_mat;
typedef std::shared_ptr<snake_mat> snake_mat_ptr;

class component {
public:
	component();
	component(snake_mat c, uint16_t x, uint16_t y);
	component(std::string str, uint16_t x, uint16_t y);

	bool operator==(component& cpnt);
	bool operator!=(component& cpnt);

	snake_mat& mat() { return _mat; };
	uint16_t& x() { return _x; };
	uint16_t& y() { return _y; };

	uint16_t upper_x() { return _x + (uint16_t)_mat[0].size(); };
	uint16_t lower_x() { return _x; };
	uint16_t upper_y() { return _y + (uint16_t)_mat.size(); };
	uint16_t lower_y() { return _y; };

	bool in_area(uint16_t x_upper, uint16_t x_lower, uint16_t y_upper, uint16_t y_lower);
	bool empty() { return _empty || _mat.size() == 0; };

	std::string to_string();

private:
	snake_mat _mat;
	uint16_t _x;
	uint16_t _y;
	bool _empty;
};

typedef std::vector<component> snake_vec;

class BaseControl {
public:
	/*BaseMatrix() : _resolution(50), _width(50), _height(25) {};*/
	explicit BaseControl(uint16_t res)
		: _resolution(res), _width(res), _height(res / 2), _components({})
	{};
	virtual void init() = 0;
	void reset_assets() { _components.clear(); init(); };

	void add_component(std::string& body, uint16_t x, uint16_t y);
	void add_component(const std::string& body, uint16_t x, uint16_t y);
	void add_component(component comp);
	
	void add_component(snake_vec& components, std::string& body, uint16_t x, uint16_t y);
	void add_component(snake_vec& components, const std::string& body, uint16_t x, uint16_t y);
	void add_component(snake_vec& components, component comp);

	//component str2cpnt(std::string str, uint16_t x, uint16_t y);
	component str2cpnt(const std::string str, uint16_t x, uint16_t y);

	snake_vec& components() { return _components; };

	void set_resolution(uint16_t res) { _resolution = res; _height = res / 2; _width = res; };
	uint16_t resolution() { return _resolution; };
	uint16_t width() { return _width; };
	uint16_t height() { return _height; };

	snake_mat init_mat();
	snake_mat init_mat(uint16_t h, uint16_t w);

	component init_fence(uint16_t height, uint16_t width);
	component init_fence(uint16_t height, uint16_t width, uint16_t x, uint16_t y);

	uint16_t upper_x();
	uint16_t lower_x();
	uint16_t upper_y();
	uint16_t lower_y();

	void shift_component(component& cpnt, bool direct_x, int16_t distance);

	//void for_each_mat(snake_mat mat, void (*do_sth)());

protected:
	snake_vec _components;
	uint16_t _resolution;
	uint16_t _width;
	uint16_t _height;
};


