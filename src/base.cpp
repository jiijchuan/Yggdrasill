#include "base.h"

component::component() : 
	_mat({ {} }), _x(0), _y(0), _empty(true) 
{};

component::component(snake_mat c, uint16_t x, uint16_t y) : 
	_mat(c), _x(x), _y(y), _empty(false) 
{};

component::component(std::string str, uint16_t x, uint16_t y) :
	_mat({ std::vector<char>(str.begin(), str.end()) }),
	_x(x), _y(y), _empty(false) 
{};

bool component::operator==(component& cpnt) {
	return this->_mat == cpnt._mat 
		&& this->_x == cpnt._x 
		&& this->_y == cpnt._y;
}

bool component::operator!=(component& cpnt) {
	return !(*this == cpnt);
}

bool component::in_area(uint16_t x_upper, uint16_t x_lower, uint16_t y_upper, uint16_t y_lower) {
	return x_lower <= x_upper
		&& y_lower <= y_upper
		&& x_lower <= _x + _mat[0].size()
		&& _x + _mat[0].size() <= x_upper
		&& y_lower <= _y + _mat.size()
		&& _y + _mat.size() <= y_upper;
}

std::string component::to_string() {
	assert(_mat.size() == 1);
	return std::string(_mat[0].begin(), _mat[0].end());
}

void BaseControl::add_component(std::string& body, uint16_t x, uint16_t y) {
	_components.push_back(str2cpnt(body, x, y));
}

void BaseControl::add_component(const std::string& body, uint16_t x, uint16_t y) {
	_components.push_back(str2cpnt(body, x, y));
}

void BaseControl::add_component(component comp) {
	_components.push_back(comp);
}

void BaseControl::add_component(snake_vec& components, std::string& body, uint16_t x, uint16_t y) {
	components.push_back(str2cpnt(body, x, y));
}

void BaseControl::add_component(snake_vec& components, const std::string& body, uint16_t x, uint16_t y) {
	components.push_back(str2cpnt(body, x, y));
}

void BaseControl::add_component(snake_vec& components, component comp) {
	components.push_back(comp);
}

component BaseControl::str2cpnt(const std::string str, uint16_t x, uint16_t y) {
	return component(snake_mat({ std::vector<char>(str.begin(), str.end()) }), x, y);
}

snake_mat BaseControl::init_mat() {
	snake_mat m;
	for (size_t i = 0; i < _height; i++) {
		std::vector<char> temp;
		for (size_t j = 0; j < _width; j++) {
			temp.push_back(SPACE);
		}
		m.push_back(temp);
	}
	return m;
}

snake_mat BaseControl::init_mat(uint16_t h, uint16_t w) {
	snake_mat m;
	for (size_t i = 0; i < h; i++) {
		std::vector<char> temp;
		for (size_t j = 0; j < w; j++) {
			temp.push_back(SPACE);
		}
		m.push_back(temp);
	}
	return m;
}

component BaseControl::init_fence(uint16_t height, uint16_t width, uint16_t x, uint16_t y) {
	auto m = init_mat(height, width);
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1) m[i][j] = FENCE;
		}
	}
	return component(m, x, y);
}

component BaseControl::init_fence(uint16_t height, uint16_t width) {
	return init_fence(height, width, 0, 0);
}

uint16_t BaseControl::upper_x() {
	auto up_x = 0;
	for (auto& i : _components) { up_x = max(i.upper_x(), up_x); };
	return up_x;
}

uint16_t BaseControl::lower_x() {
	auto lo_x = _width;
	for (auto& i : _components) { lo_x = min(i.lower_x(), lo_x); };
	return lo_x;
}

uint16_t BaseControl::upper_y() {
	auto up_y = 0;
	for (auto& i : _components) { up_y = max(i.upper_y(), up_y); };
	return up_y;
}

uint16_t BaseControl::lower_y() {
	auto lo_y = _height;
	for (auto& i : _components) { lo_y = min(i.lower_y(), lo_y); };
	return lo_y;
}

void BaseControl::shift_component(component& cpnt, bool direct_x, int16_t distance) {
	if (direct_x) cpnt.x() = cpnt.x() + distance;
	else cpnt.y() = cpnt.y() + distance;
}