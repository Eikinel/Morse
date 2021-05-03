#pragma once

# include "Constants.h"

class   Bezier
{
public:
    Bezier(
		const std::vector<sf::Vector2f>& points,
		const std::vector<float>& durations,
		const sf::Time& start,
		float& bpm,
		const sf::Color color = sf::Color::Green,
		const size_t& nbSegments = 50,
		const bool& isClosed = false);
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
	const sf::Vertex&					getVertexByTiming(const sf::Time& timing) const;
	const int							getVertexIndexByTiming(const sf::Time& timing) const;
	const sf::Time&						getTimingByIndex(const size_t& index) const;
    std::vector<sf::Vertex>&			getBezierCurve();
	const sf::Time&						getStart() const;
	const sf::Time&						getEnd() const;
	const float&						getPixelLength() const;
	const float&						getDuration() const;
	const sf::Transform&				getTransform() const;

    // SETTERS
    void setClose(const bool isClosed);
    void setSegments(const size_t nbSegments);
    void setPointById(const int id, const sf::Vector2f& pos);
	void setStart(const sf::Time& start);
	void setDuration(const float& end);
	void setTransform(const sf::Transform& transform);

    // METHODS
    void addPoint(const sf::Vector2f& point);
	void addTiming(const float& offset);
	void removeVertex(const std::vector<sf::Vertex>::const_iterator it_points);
	void removeVertexAt(const size_t& index);
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
        const sf::Vector2f& endControl
	);

    // VARIABLES
    bool                        _isClosed;
	sf::Color					_color;
    size_t                      _nbSegments;
	size_t						_nbMaxVertices;
    std::vector<sf::Vector2f>   _points;
    std::vector<sf::Vector2f>   _controlPoints;
    std::vector<sf::Vector2f>   _anchorPoints;
    std::vector<sf::Vertex>		_bezierCurve;
	sf::Time					_start;
	float						_duration;
	std::vector<float>			_durations;
	float						_pixel_length;
	float&						_bpm;
	sf::Transform				_transform;
};