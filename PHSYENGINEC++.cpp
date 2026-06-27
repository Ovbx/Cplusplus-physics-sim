#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>

int Width = 1000;
int Height = 600;
int fps = 60;
int wall_thickness = 12;
double gravity = 0.5;
double bounce_stop = 0.3;
double substeps = 1;

std::vector<sf::Vector2i> mouse_trajectory;

class Ball {
public:
int x_pos;
int y_pos;
double radius;
double mass;
double retention;
double x_speed, y_speed;
int id;
double friction;
bool selected = false;

Ball(int x_pos, int y_pos, double radius, int mass, double retention, double x_speed, double y_speed, int id, double friction) 
    : x_pos(x_pos), y_pos(y_pos), radius(radius), mass(mass), retention(retention), x_speed(x_speed), y_speed(y_speed), id(id), friction(friction)
{
    std::cout << "Ball" << id << " created\n";
}
    void drawCircle(sf::RenderWindow& window) {
        sf::CircleShape shape(radius);
        shape.setPosition({(float)(x_pos - radius), (float)(y_pos - radius)});
        shape.setFillColor(sf::Color::Blue);
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
        //floor hcekc
        if (y_pos >= height - radius - (wall_thickness / 2 )) {
            y_pos = height - radius - (wall_thickness/2.4);
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

double total_energy(std::vector<Ball>& balls) {
    double total = 0;
    for (Ball& ball:balls) {
        total += 0.5 * ball.mass * (ball.x_speed * ball.x_speed + ball.y_speed * ball.y_speed);
    }
    return total;
}

Ball ball1(200, 200, 20, 200, 0.6, 0, 0, 1, 0.02);
Ball ball2(100, 300, 100, 200, 0.6, 0, 0, 2, 0.02);

std::vector<Ball> balls = {ball1, ball2};

bool running = true;

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)Width, (unsigned)Height}), "C++ Physics Engine");
    window.setFramerateLimit(fps);
    while (window.isOpen()) {
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
            
        }

        sf::Vector2i mouse_coords = sf::Mouse::getPosition(window);
        mouse_trajectory.push_back(mouse_coords);
        if(mouse_trajectory.size() > 20) {
            mouse_trajectory.erase(mouse_trajectory.begin());
        }

    
        window.clear(sf::Color::Black);
        drawWalls(window);
        sf::Vector2u size = window.getSize();
        //draw everything else here ig
        for(auto& ball:balls) {
            ball.applyGravity(size.y);
            ball.drawCircle(window);
        }

    
        for (int i = 0; i < substeps; i++) {
            for (auto& ball: balls) {
                ball.updatePos(mouse_coords);
                ball.checkWalls(size.x, size.y);
            }
        }
        std::cout << "Ke: " << total_energy(balls) << "\n";
        window.display();
    }
}