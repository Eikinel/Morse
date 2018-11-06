#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class   Bezier
{
public:
    Bezier(
		const std::vector<sf::Vector2f>& points = {},
		const sf::Color& color = sf::Color::Green,
		const bool isClosed = false,
		const size_t nbSegments = 20);
    ~Bezier()
    {
        std::cout << "Destruct Bezier" << std::endl;
    }

    // GETTERS
    const bool                          isClosed() const;
    const size_t                        getSegments() const;
    const std::vector<sf::Vector2f>&    getPoints() const;
    const std::vector<sf::Vector2f>&    getControlPoints() const;
    const std::vector<sf::Vector2f>&    getAnchorPoints() const;
    const sf::VertexArray&              getBezierCurve() const;

    // SETTERS
    void setClose(const bool isClosed);
    void setSegments(const size_t nbSegments);
    void setPointById(const int id, const sf::Vector2f& pos);

    // METHODS
    void addPoint(const sf::Vector2f& point);
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
    std::vector<sf::Vector2f>   _points;
    std::vector<sf::Vector2f>   _controlPoints;
    std::vector<sf::Vector2f>   _anchorPoints;
    sf::VertexArray             _bezierCurve;
};