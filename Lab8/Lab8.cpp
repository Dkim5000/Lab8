// Lab8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunt");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    Texture redTex;
    LoadTex(redTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> balloons;
    for (int i(0); i < 6; i++) {
        PhysicsSprite& balloon = balloons.Create();
        balloon.setTexture(redTex);
        int x = 50 + ((700 / 5) * i);
        Vector2f sz = balloon.getSize();
        balloon.setCenter(Vector2f(x, 20 + (sz.y / 2)));
        balloon.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(balloon);
        balloon.onCollision =
            [&drawingArrow, &world, &arrow, &balloon, &balloons, &score]
            (PhysicsBodyCollisionResult result) {
            if (result.object2 == arrow) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(balloon);
                balloons.QueueRemove(balloon);
                score += 10;
            }
            };
    }

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("comic.ttf")) {
        cout << "Couldn't load font sans.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    Text arrowCountText;
    arrowCountText.setFont(font);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    long duckMS(0);
    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        duckMS = duckMS + deltaMS; //increment our duck counter
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
                arrows -= 1;
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            balloons.DoRemovals();
            for (PhysicsShape& balloon : balloons) {
                window.draw((PhysicsSprite&)balloon);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(790 - textBounds.width, 590 - textBounds.height));
            window.draw(scoreText);
            arrowCountText.setString(to_string(arrows));
            textBounds = arrowCountText.getGlobalBounds();
            arrowCountText.setPosition(
                Vector2f(10, 590 - textBounds.height));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();
            balloons.DoRemovals();
        }
        if (duckMS > 600) { //2000 MS is 2 seconds
            duckMS = 0;
            PhysicsSprite& balloon = balloons.Create();
            balloon.setTexture(redTex);
            int x = 50 + ((700 / 5));
            Vector2f sz = balloon.getSize();
            balloon.setCenter(Vector2f(-100, 20 + (sz.y / 2)));
            balloon.setVelocity(Vector2f(0.25, 0));
            world.AddPhysicsBody(balloon);
            balloon.onCollision =
                [&drawingArrow, &world, &arrow, &balloon, &balloons, &score]
                (PhysicsBodyCollisionResult result) {
                if (result.object2 == arrow) {
                    drawingArrow = false;
                    world.RemovePhysicsBody(arrow);
                    world.RemovePhysicsBody(balloon);
                    balloons.QueueRemove(balloon);
                    score += 10;
                }
                };
        }
    }
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(
        400 - (textBounds.width / 2),
        300 - (textBounds.height / 2)
    ));
    window.draw(gameOverText);
    window.display();
    while (true){
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            exit(0);
        }
    }
}