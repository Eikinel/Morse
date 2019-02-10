#pragma once

# include "Constants.h"

class   Bezier
{
public:
    Bezier(
		const std::vector<sf::Vector2f>& points,
		const float& timing,
		const size_t nbSegments = 200,
		const sf::Color& color = sf::Color::Green,
		const bool isClosed = false);
    ~Bezier()
    {
        std::cout << "Destruct Bezier" << std::endl;
    }

    // GETTERS
    const bool                          isClosed() const;
	const size_t                        getSegments() const;
	const size_t                        getMaxVertices() const;
    const std::vector<sf::Vector2f>&    getPoints() const;
    const std::vector<sf::Vector2f>&    getControlPoints() const;
    const std::vector<sf::Vector2f>&    getAnchorPoints() const;
    std::vector<sf::Vertex>&			getBezierCurve();
	const sf::Time&						getTiming() const;
	const float&						getPixelLength() const;

    // SETTERS
    void setClose(const bool isClosed);
    void setSegments(const size_t nbSegments);
    void setPointById(const int id, const sf::Vector2f& pos);
	void setTiming(const float timing);

    // METHODS
    void addPoint(const sf::Vector2f& point);
	void addTiming(const float& offset);
	void removeVertex(const std::vector<sf::Vertex>::const_iterator it_point);
    void updateControlPoints();
    void updateAnchorPoints();
    void updateBezierCurve();
    void update();

private:
    // METHODS
    void addControlPoint(const sf::Vector2f& cpoint);
    void addAnchorPoint(const sf::Vector2f& apoint);
    void cubicBezierVertex(
        const sf::Vector2f& startAnchor,
        const sf::Vector2f& endAnchor,
        const sf::Vector2f& startControl,
        const sf::Vector2f& endControl);

    // VARIABLES
    bool                        _isClosed;
	sf::Color					_color;
    size_t                      _nbSegments;
	size_t						_nbMaxVertices;
    std::vector<sf::Vector2f>   _points;
    std::vector<sf::Vector2f>   _controlPoints;
    std::vector<sf::Vector2f>   _anchorPoints;
    std::vector<sf::Vertex>		_bezierCurve;
	sf::Time					_timing;
	float						_length;
};