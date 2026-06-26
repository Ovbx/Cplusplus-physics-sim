#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>


int Width = 1000;
int Height = 800;

int fps = 60;
int wall_thickness = 10;
double gravity = 0.5;
double bounce_stop = 0.3;
double substeps = 4;

std::vector<sf::Vector2i> mouse_trajectory;

class Ball {
private:
int x_pos;
int y_pos;
double radius;
double mass;
double retention;
double x_speed, y_speed;
int id;
double friction;

public:
Ball(int x_pos, int y_pos, double radius, int mass, double retention, double x_speed, double y_speed, int id, double friction) 
    : x_pos(x_pos), y_pos(y_pos), radius(radius), mass(mass), retention(retention), x_speed(x_speed), y_speed(y_speed), id(id), friction(friction)
{
    std::cout << "Ball" << id << " created\n";
}
    void draw(sf::RenderWindow& window) {
        sf::CircleShape shape(radius);
        shape.setPosition({(float)(x_pos - radius), (float)(y_pos - radius)});
        shape.setFillColor(sf::Color::Blue);
        window.draw(shape);
    }
    double checkGravity() {

        return 0;
    }
    void updatePos(double substeps) {
        y_pos += y_speed / substeps;
        x_pos += x_speed / substeps;
    }

};


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

std::vector<Ball> balls = {ball1};

bool running = true;

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)Width, (unsigned)Height}), "Physics Engine");
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
        if(mouse_trajectory.size() < 20) {
            mouse_trajectory.erase(mouse_trajectory.begin());
        }

    
        window.clear(sf::Color::Black);
        drawWalls(window);
        //draw everything else here ig
        window.display();
    }
}