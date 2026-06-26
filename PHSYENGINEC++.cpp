#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>

int Width = 1000;
int Height = 600;
int fps = 60;
int wall_thickness = 10;
double gravity = 0.5;
double bounce_stop = 0.3;
double substeps = 4;

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
    double checkGravity(float width, float height, double x_push, double y_push) {
        if (!selected) {
            if (y_pos < height - radius - (wall_thickness / 2)) {
                y_speed += gravity;
            }
            else {
                if (y_speed > bounce_stop) {
                    y_speed = y_speed * -1 * retention;
                }
                else {
                    if(std::abs(y_speed) <= bounce_stop) {
                        y_speed = 0;
                    }
                }
                if ((x_pos < radius + (wall_thickness/2) && x_speed < 0) || x_pos > width - radius - (wall_thickness / 2 ) && x_speed > 0) {
                    x_speed *= -1 * retention;
                    if (std::abs(x_speed) < bounce_stop) {
                        x_speed = 0;
                    }
                }
                if (y_speed ==0 && x_speed != 0) {
                    if (x_speed > 0) {
                        x_speed -= friction;
                    }
                    if (x_speed < 0) {
                        x_speed += friction;
                    }
                }
            }
            
        }
        else {
            x_speed = x_push;
            y_speed = y_push;
        }
        return y_speed;
    }
    void updatePos(double substeps, sf::Vector2i mouse) {
        if(!selected) {
            y_pos += y_speed / substeps;
            x_pos += x_speed / substeps;
        }
        else {
            x_pos = mouse.x;
            y_pos = mouse.y;
        }
    }
    bool check_select() {
        //need work
        selected = false;
        return selected;
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
Ball ball1(200, 200, 20, 200, 0.9, 0, 0, 1, 0.02);
Ball ball2(100, 300, 30, 200, 0.9, 0, 0, 2, 0.02);

std::vector<Ball> balls = {ball1, ball2};

bool running = true;

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)Width, (unsigned)Height}), "C++ Physics Engine");
    window.setFramerateLimit(fps);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect area({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(area));
            }
        }

        sf::Vector2i mouse_coords = sf::Mouse::getPosition(window);
        mouse_trajectory.push_back(mouse_coords);
        if(mouse_trajectory.size() > 20) {
            mouse_trajectory.erase(mouse_trajectory.begin());
        }
        sf::Vector2f push = calc_motion_vector();

    
        window.clear(sf::Color::Black);
        drawWalls(window);
        //draw everything else here ig
        for(auto& ball:balls) {
            ball.drawCircle(window);
            sf::Vector2u size = window.getSize();
            ball.checkGravity(size.x, size.y, push.x, push.y);
        }
        for (int i = 0; i < substeps; i++) {
            for (auto & ball: balls) {
                ball.updatePos(substeps, mouse_coords);
            }
        }
        window.display();
    }
}