#pragma once

# include "Constants.h"

class		Curve
{
public:
	Curve(float duration, const sf::Vector2f& vstart, const sf::Vector2f& vend);
	~Curve() { std::cout << "Deleting curve" << std::endl; }

	//GETTERS
	const std::vector<sf::Vector2f>&	getControlPoints() const;
	const sf::VertexArray&				getBezierCurve() const;

	//METHODS
	void	scaleCurve(const unsigned int& speed);

private:
	sf::VertexArray				_bezier_curve;
	std::vector<sf::Vector2f>	_control_points;

	//METHODS
	std::vector<sf::Vector2f> calcBezier(const sf::Vector2f& start, const sf::Vector2f& end, const size_t num_segments);
};