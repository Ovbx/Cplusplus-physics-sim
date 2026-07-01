#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>
#include <set>

int Width = 1000;
int Height = 600;
int fps = 60;
int wall_thickness = 12;
double gravity = 0.5;
double bounce_stop = 0.3;
int substeps = 3;
int solverInterations = 6;
double retention = 0.9;
double maxRadius = 50;


std::vector<sf::Vector2i> mouse_trajectory;


class Ball {
public:
double x_pos;
double y_pos;
double radius;
double mass;
double retention;
double x_speed, y_speed;
int id;
double friction;
bool selected = false;

Ball(double x_pos, double y_pos, double radius, int mass, double retention, double x_speed, double y_speed, int id, double friction) 
    : x_pos(x_pos), y_pos(y_pos), radius(radius), mass(mass), retention(retention), x_speed(x_speed), y_speed(y_speed), id(id), friction(friction)
{
    std::cout << "Ball" << id << " created\n";
}
    void drawCircle(sf::RenderWindow& window) {
        sf::CircleShape shape(radius);
        shape.setPosition({(float)(x_pos - radius), (float)(y_pos - radius)});
        double speed = std::sqrt(x_speed * x_speed + y_speed * y_speed);
        double maxSpeed = 30;
        double normalized = speed / maxSpeed;
        if (normalized > 1.0) {
            normalized = 1;
        }
        int red = (int)(normalized * 255);
        shape.setFillColor(sf::Color(red, 0, 255 - red));

        window.draw(shape);
    }
    void applyGravity(float height) {
        if (selected)
        return;
        bool onFloor = (y_pos >= height - radius - wall_thickness / 2) && std::abs(y_speed) < bounce_stop;
        if (!onFloor)
            y_speed += gravity;
        
    }
    void updatePos(sf::Vector2i mouse) {
        if(!selected) {
            y_pos += y_speed / substeps;
            x_pos += x_speed / substeps;
        }
        else {
            x_pos = mouse.x;
            y_pos = mouse.y;
        }
    }
    bool check_select(sf::Vector2i mouse) {
        double dx = mouse.x - x_pos;
        double dy = mouse.y - y_pos;
        double dist = std::sqrt(dx*dx + dy*dy);
        selected = (dist <= radius);
        return selected;
    }
    void checkWalls(float width, float height) {
        if (selected)
        return;
        //ceiling
        if (y_pos <=radius + wall_thickness / 2.0) {
            y_pos = radius + wall_thickness /2.0;
            if (y_speed < 0) {
                y_speed = y_speed * -1 * retention;
            }
        }
        //floor hcekc
        if (y_pos >= height - radius - (wall_thickness / 2 )) {
            y_pos = height - radius - (wall_thickness/2.45);
            if (y_speed > bounce_stop)
                y_speed = y_speed * -1 * retention;
            else if (std::abs(y_speed) <= bounce_stop)
                y_speed = 0;
        }

        //wall check
        if (x_pos < radius + wall_thickness/2) {
            x_pos = radius + wall_thickness / 2;
            if (x_speed < 0)
            x_speed *= -1 * retention;
        }
        if (x_pos > width - radius - wall_thickness/2 ) {
            x_pos = width - radius - wall_thickness/2;
            if (x_speed > 0) 
            x_speed *= -1 * retention;
        }
        if (y_speed == 0 && x_speed != 0) {
            if (std::abs(x_speed) <= friction) {
                x_speed = 0;
            }
            else if (x_speed > 0) {
                x_speed -= friction;
            }
            else {
                x_speed += friction;
            }
        }
    }

};

class QuadTree {
public:
    sf::FloatRect boundary;
    int capacity = 4;
    std::vector <Ball*> balls;
    bool divided = false;
    QuadTree* nw = nullptr;
    QuadTree* ne = nullptr;
    QuadTree* sw = nullptr;
    QuadTree* se = nullptr;

QuadTree(sf::FloatRect boundary, int capacity) : boundary(boundary), capacity(capacity)
{
}
~QuadTree() {
    delete nw;
    delete ne;
    delete sw;
    delete se;

}

bool contains(Ball& ball) {
    return boundary.contains(sf::Vector2f(ball.x_pos, ball.y_pos));
}

void subdivide() {
    float hw = boundary.size.x / 2;
    float hh = boundary.size.y / 2;
    nw = new QuadTree(sf::FloatRect({boundary.position.x, boundary.position.y}, {hw, hh}), capacity);
    ne = new QuadTree(sf::FloatRect({boundary.position.x + hw, boundary.position.y}, {hw, hh}), capacity);
    sw = new QuadTree(sf::FloatRect({boundary.position.x, boundary.position.y + hh}, {hw, hh}), capacity);
    se = new QuadTree(sf::FloatRect({boundary.position.x + hw, boundary.position.y + hh}, {hw, hh}), capacity);
    divided = true;
}
bool intersects(sf::FloatRect area) {
    float bx = boundary.position.x;
    float by = boundary.position.y;
    float bw = boundary.size.x;
    float bh = boundary.size.y;

    float ax = area.position.x;
    float ay = area.position.y;
    float aw = area.size.x;
    float ah = area.size.y;
    
    return !(ax > bx + bw || ax + aw < bx || ay > by + bh || ay + ah < by);
}
std::vector<Ball*> query(sf::FloatRect area, std::vector<Ball*> found = {}) {
    if (!intersects(area))
        return found;
    float rx = area.position.x;
    float ry = area.position.y;
    float rw = area.size.x;
    float rh = area.size.y;
    for (Ball* ball: balls) {
        if (rx <= ball->x_pos && ball->x_pos <= rx + rw && ry <= ball->y_pos && ball->y_pos <= ry + rh) {
            found.push_back(ball);
        }
    }
    if (divided) {
        found = nw->query(area, found);
        found = ne->query(area, found);
        found = sw->query(area, found);
        found = se->query(area,found);
    }
    return found;

}
bool insert(Ball* ball) {
    if (!contains(*ball)) {
        return false;
    }
    if (balls.size() < capacity) {
        balls.push_back(ball);
        return true;
    }
    if (!divided) {
        subdivide();
    }
    return (nw->insert(ball) || ne->insert(ball) || se->insert(ball) || sw->insert(ball));
}
};
Ball ball1(200, 200, 20, 200, 0.9, 0, 0, 1, 0.02);
Ball ball2(100, 300, 30, 200, 0.9, 0, 0, 2, 0.02);
int nextId = 3;
std::vector<Ball> balls = {ball1, ball2};

sf::Vector2f calc_motion_vector() {
    float x = 0;
    float y = 0;
    if(mouse_trajectory.size() > 10) {
        x = (float)(mouse_trajectory.back().x - mouse_trajectory.front().x) / mouse_trajectory.size();
        y = (float)(mouse_trajectory.back().y - mouse_trajectory.front().y) / mouse_trajectory.size();
    }
    return {x, y};
}
void drawWalls(sf::RenderWindow& window) {
    sf::RectangleShape mainRect;
    sf::Vector2f windowSize = sf::Vector2f(window.getSize());
    mainRect.setSize(windowSize - sf::Vector2f(wall_thickness, wall_thickness));
    mainRect.setPosition({wall_thickness / 2.f, wall_thickness / 2.f});
    mainRect.setFillColor(sf::Color::Transparent);
    mainRect.setOutlineColor(sf::Color::White);
    mainRect.setOutlineThickness(wall_thickness);
    window.draw(mainRect);
}
void resolve_collision(Ball& a, Ball& b, double width, double height, double retention) {
    double dx = b.x_pos - a.x_pos;
    double dy = b.y_pos - a.y_pos;
    double dist = std::sqrt(dx * dx + dy * dy);
    double nx, ny;
    if (dist < 0.0001) {
        nx = 0;
        ny = 1;
        dist = 0.0001;
    }
    else {
        nx = dx / dist;
        ny = dy/ dist;
    }
    //dot
    double aNormal = a.x_speed * nx + a.y_speed * ny;
    double bNormal = b.x_speed * nx + b.y_speed * ny;

    double da = bNormal - aNormal;
    if (da > 0) {
        return;
    }
    double m1 = a.mass;
    double m2 = b.mass;
    double overlap = (a.radius + b.radius) - dist;
    double beta = 0.15;
    double slop = 0.9;
    double dt = 1.0;
    double e = std::min(a.retention, b.retention);

    double bias = beta * std::max(0.0, overlap - slop) / dt;
    double retentionThreshold = 1.5;
    if (std::abs(da) < retentionThreshold)
        e = 0.0;
    double impulse = ((1.0+e)*da -bias) / (m1 + m2);
    a.x_speed += impulse * m2 * nx;
    a.y_speed += impulse * m2 * ny;
    b.x_speed -= impulse * m1 * nx;
    b.y_speed -= impulse * m1 * ny;

    //overlap stuff
    if (overlap > slop) {
        m1 = a.mass;
        m2 = b.mass;
        double correction = (overlap-slop) * 0.8;
        double total = m1 + m2;
        a.x_pos -= correction * (m2 / total) * nx;
        a.y_pos -= correction * (m2 / total) * ny;
        b.x_pos += correction * (m1 / total) * nx;
        b.y_pos += correction * (m1 / total) * ny;
    }

    for (Ball* obj : {&a, &b}) {
        obj->x_pos = std::max(obj->radius + wall_thickness / 2.0, std::min(width - obj->radius - wall_thickness / 2.0, obj->x_pos));
        obj->y_pos = std::max(obj->radius + wall_thickness/2.0, std::min(height - obj->radius - wall_thickness/2.0, obj->y_pos));
    }
}
double total_energy(std::vector<Ball>& balls) {
    double total = 0;
    for (Ball& ball:balls) {
        total += 0.5 * ball.mass * (ball.x_speed * ball.x_speed + ball.y_speed * ball.y_speed);
    }
    return total;
}

bool running = true;

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)Width, (unsigned)Height}), "C++ Physics Engine");
    window.setFramerateLimit(fps);
    while (window.isOpen()) {
        sf::Vector2i mouse_coords = sf::Mouse::getPosition(window);
        sf::Vector2f push = calc_motion_vector();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect area({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(area));
            }
            if ( const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    for (auto& ball: balls) {
                        ball.check_select(click->position);
                    }
                }   
            }
            if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (released->button == sf::Mouse::Button::Left) {
                    for (auto& ball: balls) {
                        if (ball.selected) {
                            ball.x_speed = push.x;
                            ball.y_speed = push.y;
                        }
                        ball.selected = false;
                    }
                }
            }
            if (const auto* spawnBall = event->getIf<sf::Event::MouseWheelScrolled>()) {
                balls.push_back(Ball(mouse_coords.x, mouse_coords.y, 5, 5, retention, 0, 0, nextId, 0.02));
                nextId++;
            }
            
        }

        mouse_trajectory.push_back(mouse_coords);
        if(mouse_trajectory.size() > 20) {
            mouse_trajectory.erase(mouse_trajectory.begin());
        }

    
        window.clear(sf::Color::Black);
        drawWalls(window);
        sf::Vector2u size = window.getSize();
        for(auto& ball:balls) {
            ball.applyGravity(size.y);
        }

        for (int i = 0; i < substeps; i++) {
            for (auto& ball: balls) {
                ball.updatePos(mouse_coords);
                ball.checkWalls(size.x, size.y);
            }
            QuadTree qt(sf::FloatRect({0.f, 0.f}, {(float)size.x, (float)size.y}), 4);
            for (auto& ball:balls)
                qt.insert(&ball);
            for (int iter = 0; iter < solverInterations; iter++) {
                
                std::set<std::pair<int, int>> checked;

                for (auto&ball: balls) {
                    double r = ball.radius + maxRadius;
                    sf::FloatRect area({(float)(ball.x_pos - r), (float)(ball.y_pos - r)}, {(float)(2*r), (float)(2*r)});
                    std::vector<Ball*> nearby = qt.query(area);
                    for (auto* other: nearby) {
                        if (other->id <= ball.id)
                            continue;
                        double dx = other->x_pos - ball.x_pos;
                        double dy = other->y_pos - ball.y_pos;
                        double dist = std::sqrt(dx * dx + dy * dy);
                        if (dist < ball.radius + other->radius) 
                            resolve_collision(ball, *other, size.x, size.y, retention);
                    }
                }
            }
        }
        for (auto & ball: balls) {
            ball.drawCircle(window);
        }
        std::cout << total_energy(balls) << std::endl;
        window.display();
    }
}