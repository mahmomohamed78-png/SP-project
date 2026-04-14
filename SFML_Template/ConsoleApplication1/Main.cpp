#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace sf;

struct design
{
    Texture texture;
    Sprite sprite;
};


struct character {
    const int frameWidth = 160;
    int frameHeight; //boy 140, girl 128 

    Sprite sprite;
    void sprite_origin() {
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    }

    float speed_y;
    float speed_x;
    float gravity;
    float jump_strength;
    float delay;
    float timer;

    bool onground;
    bool ismoving;
    bool stop;

    int framecounter;
    int totalFrames;

    Keyboard::Key left;
    Keyboard::Key right;
    Keyboard::Key up;
};

void moveCharacter(character& player, float deltaTime)
{
    player.ismoving = false;
    if (Keyboard::isKeyPressed(player.left)) {

        player.ismoving = true;
        player.sprite.move(-player.speed_x * deltaTime, 0);
        player.sprite.setScale(-1, 1);

    }
    else if (Keyboard::isKeyPressed(player.right)) {

        player.ismoving = true;
        player.sprite.move(player.speed_x * deltaTime, 0);
        player.sprite.setScale(1, 1);
    }
}
void jumpCharacter(character& c, float deltaTime)
{
    if (Keyboard::isKeyPressed(c.up) && c.onground) {
        c.speed_y = c.jump_strength;
        c.onground = false;
    }
    if (!c.onground) {
        c.speed_y += c.gravity * deltaTime;
        c.sprite.move(0, c.speed_y * deltaTime);
    }
}
void animation(character& c, float deltaTime)
{
    if (!c.ismoving) {
        c.framecounter = 0;
        c.sprite.setTextureRect(IntRect(0, 0, c.frameWidth, c.frameHeight));
        return;
    }
    c.timer += deltaTime;
    if (c.timer >= c.delay) {
        c.timer = 0;
        c.framecounter++;
    }
    if (c.framecounter >= c.totalFrames) {
        c.framecounter = 0;
    }
    c.sprite.setTextureRect(
        IntRect(c.framecounter * c.frameWidth, 0, c.frameWidth, c.frameHeight)
    );

}

void ground_collision(character& player, RectangleShape& ground) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    if (hitbox.intersects(ground.getGlobalBounds())) {
        player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y - (player.frameHeight / 2.0));
        player.speed_y = 0;
        player.onground = true;
    }
}

void platform_collision(character& player, RectangleShape& ground) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    if (hitbox.intersects(ground.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = ground.getPosition().y;
        if (player.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y - (player.frameHeight / 2.0));
            player.speed_y = 0;
            player.onground = true;
        }
        if (player.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y + (ground.getSize().y) + (player.frameHeight / 2.0));
            player.speed_y = 0;
        }
    }
}

void died(character player) {

}

void fire_collision(character& fireBoy, character& waterGirl , RectangleShape& fire) {
    FloatRect hitbox = fireBoy.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = waterGirl.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(fire.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = fire.getPosition().y;
        if (fireBoy.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, fire.getPosition().y - (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
            fireBoy.onground = true;
        }
        if (fireBoy.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, fire.getPosition().y + (fire.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
    if (hitbox2.intersects(fire.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = fire.getPosition().y;
        if (waterGirl.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            died(waterGirl);
        }
        if (waterGirl.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, fire.getPosition().y + (fire.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
}

void water_collision(character& fireBoy, character& waterGirl, RectangleShape& water) {
    FloatRect hitbox = waterGirl.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = fireBoy.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(water.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = water.getPosition().y;
        if (waterGirl.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, water.getPosition().y - (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
            waterGirl.onground = true;
        }
        if (waterGirl.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, water.getPosition().y + (water.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
    if (hitbox2.intersects(water.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = water.getPosition().y;
        if (fireBoy.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
			died(fireBoy);
        }
        if (fireBoy.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, water.getPosition().y + (water.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
}

void slope_collision(character& player, double x1, double y1, double x2, double y2) {
    double player_x = player.sprite.getPosition().x;
    if (player_x >= x1 && player_x <= x2 && player.sprite.getPosition().y + (player.frameHeight / 2.0f) <= y1) {
        double slopeY = y1 + (player_x - x1)* ((y2 - y1) / (x2 - x1));
        if ((player.sprite.getPosition().y + (player.frameHeight / 2.0f)) >= slopeY) {
            player.sprite.setPosition(player_x, slopeY - (player.frameHeight / 2.0f));
            player.speed_y = 0;
            player.onground = true;
        }
    }
}

void wall_collision(character& player, RectangleShape& wall) {
    if (player.sprite.getGlobalBounds().intersects(wall.getGlobalBounds())) {
        if (player.sprite.getPosition().x < wall.getPosition().x && player.sprite.getScale().x == 1) {
            player.stop = true;
        }
        else if ((player.sprite.getPosition().x + wall.getSize().x) > wall.getPosition().x && player.sprite.getScale().x == -1) {
            player.stop = true;

        }
        else {
            player.stop = false;
        }
    }
    if (player.stop) {
        player.speed_x = 0;
    }
    else {
        player.speed_x = 450.0f;
    }
}

//void box_collision(character& player,design& box, RectangleShape& wall,float dt) {
//    bool stop=0;
//    float speed;
//    if (box.sprite.getGlobalBounds().intersects(wall.getGlobalBounds())) {
//        if (box.sprite.getPosition().x < wall.getPosition().x && player.sprite.getScale().x == 1) {
//            stop = true;
//        }
//        else if ((box.sprite.getPosition().x + wall.getSize().x) > wall.getPosition().x && player.sprite.getScale().x == -1) {
//            stop = true;
//
//        }
//        else {
//            stop = false;
//        }
//    }
//    if (stop) {
//        speed = 0;
//    }
//    else {
//        speed = player.speed_x*dt;
//    }
//    if (player.sprite.getGlobalBounds().intersects(box.sprite.getGlobalBounds()) && player.sprite.getScale().x == 1)
//        box.sprite.move(speed, 0);
//    else if (player.sprite.getGlobalBounds().intersects(box.sprite.getGlobalBounds()) && player.sprite.getScale().x == -1)
//        box.sprite.move(-speed, 0);
//    else if (player.sprite.getGlobalBounds().intersects(box.sprite.getGlobalBounds()) && player.sprite.getPosition().y == box.sprite.getPosition().y - (player.frameHeight / 2))
//        player.sprite.setPosition(player.sprite.getPosition().x, box.sprite.getPosition().y - (player.frameHeight / 2));
//}


void origin(RectangleShape& x)
{
    x.setOrigin(x.getLocalBounds().width / 2, x.getLocalBounds().height / 2);
}

design Frame;
design background;

int main()
{
    int x = 5;
    //definitions level1
    design ground[12];
    design Platform[2];
    design fire_lake[3];
    design water_lake[2];
    design box;
    design triangle;
    design fire_point[3];
    design water_point[3];
    RectangleShape collision_boxs[12];
    RectangleShape walls[4];
    RectangleShape lakes[5];

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load level1
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    ground[0].texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    ground[1].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[2].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[3].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[4].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[5].texture.loadFromFile("game_textures\\New_Piskel_14\\New_Piskel_14.png");
    ground[6].texture.loadFromFile("game_textures\\Tile_03.png");
    ground[7].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[8].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[9].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[10].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[11].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    Platform[0].texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    Platform[1].texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    fire_lake[0].texture.loadFromFile("game_textures/sprite_01/sprite_0.png");
    fire_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    fire_lake[2].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    water_lake[0].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    water_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    box.texture.loadFromFile("game_textures\\stone.png");
    triangle.texture.loadFromFile("game_textures\\Triangle\\sprite_tri0.png");
    fire_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    water_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Sprite level1
    Frame.sprite.setTexture(Frame.texture);
    background.sprite.setTexture(background.texture);
    box.sprite.setTexture(box.texture);
    for (int i = 0; i < 12; i++)
        ground[i].sprite.setTexture(ground[i].texture);


    Platform[0].sprite.setTexture(Platform[0].texture);
    Platform[1].sprite.setTexture(Platform[1].texture);

    for (int i = 0; i < 3; i++)
        fire_lake[i].sprite.setTexture(fire_lake[i].texture);

    for (int i = 0; i < 2; i++)
        water_lake[i].sprite.setTexture(water_lake[i].texture);

    triangle.sprite.setTexture(triangle.texture);
    for (int i = 0; i < 3; i++)
        fire_point[i].sprite.setTexture(fire_point[i].texture);

    for (int i = 0; i < 3; i++)
        water_point[i].sprite.setTexture(water_point[i].texture);


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);


    //ground0
    ground[0].sprite.setScale(3, 1.2);
    ground[0].sprite.setPosition(200, 1000 - 691.2 + 100-80);


    //// ground1
    ground[1].sprite.setScale(0.875, 1.5);
    ground[1].sprite.setPosition(1220, 1000);


    // ground2
    ground[2].sprite.setPosition(0, 1000);
    ground[2].sprite.setScale(0.875, 1.5);


    // ground3
    ground[3].sprite.setPosition(1920 - 105.6 - 182, 700+20 );


    // ground4
    ground[4].sprite.setScale(0.5, 1);
    ground[4].sprite.setPosition(296, 650 + 50-80);


    // ground5
    ground[5].sprite.setPosition(744 - 128.4, 650 - 64 + 50-80);
	ground[5].sprite.setScale(1.2, 2);


    //ground6
    ground[6].sprite.setScale(2, 2);
    ground[6].sprite.setPosition(1448 - 172.8, 650 + 50-80);


    //ground7
    ground[7].sprite.setScale(0.5, 1);
    ground[7].sprite.setPosition(296, 1000 - 691.2 + 100-80);


    //ground8
    ground[8].sprite.setPosition(1920 - 105.6 - 182, 450-80);


    //ground9
    ground[9].sprite.setPosition(506 - 50, 300-80);


    //ground10
    ground[10].sprite.setScale(0.5, 1);
    ground[10].sprite.setPosition(932.6, 300-80);


    //ground11
    ground[11].sprite.setScale(-1, 1);
    ground[11].sprite.setPosition(1545 + 20.6, 300-80);


    //Platform0
    Platform[0].sprite.setPosition(848, 1080 - 128 - 64 - 10);

    //platform1
    Platform[1].sprite.setPosition(1450, 850);
    Platform[1].sprite.setScale(1/1.5, 1);


    // box
    box.sprite.setPosition(1700-400, 1000 - 64 - 23);
 

    // Triengle
    triangle.sprite.setScale(2, 2);
    triangle.sprite.setPosition(1448 - 70, 500 + 50-80);


    // Fire lake 1 
    fire_lake[0].sprite.setPosition(700, 1000);
    fire_lake[0].sprite.setScale(1.82, 2);


    // Fire lake 2 
    fire_lake[1].sprite.setScale(0.7, 2);
    fire_lake[1].sprite.setPosition(1160 - 86.4, 650 + 50 - 80);


    // Fire lake 3
    fire_lake[2].sprite.setScale(0.7, 2);
    fire_lake[2].sprite.setPosition(1092.6, 300-80);


    // Water lake 1
    water_lake[0].sprite.setScale(0.7, 2);
    water_lake[0].sprite.setPosition(456, 650 + 50-80);


    // Water lake 2
    water_lake[1].sprite.setScale(0.7, 2);
    water_lake[1].sprite.setPosition(731, 300-80);


    //fire_point1
    fire_point[0].sprite.setScale(0.1, 0.1);
    fire_point[0].sprite.setPosition(940, 950);


    //fire_point2
    fire_point[1].sprite.setScale(0.1, 0.1);
    fire_point[1].sprite.setPosition(350, 700 - 55-90);


    //fire_point3
    fire_point[2].sprite.setScale(0.1, 0.1);
    fire_point[2].sprite.setPosition(456 + 137, 300 - 55-90);

    //water_point1
    water_point[0].sprite.setScale(0.1, 0.1);
    water_point[0].sprite.setPosition(950-10, 856 - 23-20);


    //water_point2
    water_point[1].sprite.setScale(0.1, 0.1);
    water_point[1].sprite.setPosition(865.6, 600 - 22-90);


    //water_point3
    water_point[2].sprite.setScale(0.1, 0.1);
    water_point[2].sprite.setPosition(932.6 + 80, 300 - 55-90);


    for (int i = 0; i < 12; i++) {
		Vector2f vector(ground[i].sprite.getGlobalBounds().width, ground[i].sprite.getGlobalBounds().height);
		collision_boxs[i].setSize(vector);
		collision_boxs[i].setPosition(ground[i].sprite.getPosition());
    }
	collision_boxs[11].setPosition(ground[11].sprite.getPosition().x-ground[11].sprite.getGlobalBounds().width, ground[11].sprite.getPosition().y);
	collision_boxs[0].setSize(Vector2f(15, ground[0].sprite.getGlobalBounds().height));
    collision_boxs[0].setPosition(ground[0].sprite.getPosition().x + 42, ground[0].sprite.getPosition().y);
	collision_boxs[5].setSize(Vector2f(ground[5].sprite.getGlobalBounds().width - 152, ground[5].sprite.getGlobalBounds().height));
	collision_boxs[5].setPosition(ground[5].sprite.getPosition().x + 76, ground[5].sprite.getPosition().y);
    collision_boxs[8].setSize(Vector2f(ground[8].sprite.getGlobalBounds().width - 64, ground[8].sprite.getGlobalBounds().height));
    collision_boxs[8].setPosition(ground[8].sprite.getPosition().x + 64, ground[8].sprite.getPosition().y);
    collision_boxs[9].setSize(Vector2f(ground[9].sprite.getGlobalBounds().width - 64, ground[9].sprite.getGlobalBounds().height));
    collision_boxs[9].setPosition(ground[9].sprite.getPosition().x + 64, ground[9].sprite.getPosition().y);
    collision_boxs[3].setSize(Vector2f(ground[3].sprite.getGlobalBounds().width - 64, ground[3].sprite.getGlobalBounds().height));
    collision_boxs[3].setPosition(ground[3].sprite.getPosition().x + 64, ground[3].sprite.getPosition().y);
    collision_boxs[11].setSize(Vector2f(ground[11].sprite.getGlobalBounds().width - 64, ground[11].sprite.getGlobalBounds().height));
    
    
    walls[0].setSize(Vector2f(20, 1080));
	walls[0].setPosition(-20, 0);
	walls[0].setOrigin(walls[0].getLocalBounds().width , 0);
	walls[1].setSize(Vector2f(20, 1080));
	walls[1].setPosition(1890 + 50, 0);
	walls[2].setSize(Vector2f(1920, 40));
	walls[2].setPosition(0, 0);
	walls[3].setSize(Vector2f(Platform[0].sprite.getGlobalBounds().width ,Platform[0].sprite.getGlobalBounds().height)); // changing the position
	walls[3].setPosition(Platform[0].sprite.getPosition().x, Platform[0].sprite.getPosition().y);
    walls[3].setFillColor(Color::Blue);

    for (int i = 0; i < 2; i++) {
        Vector2f vect(water_lake[i].sprite.getGlobalBounds().width, water_lake[i].sprite.getGlobalBounds().height/3);
        lakes[i].setSize(vect);
        lakes[i].setPosition(water_lake[i].sprite.getPosition().x, water_lake[i].sprite.getPosition().y + (water_lake[i].sprite.getGlobalBounds().height * 2 / 3));
    }
    for (int i = 0; i < 3; i++) {
        Vector2f vect(fire_lake[i].sprite.getGlobalBounds().width, fire_lake[i].sprite.getGlobalBounds().height/3);
        lakes[i+2].setSize(vect);
        lakes[i + 2].setPosition(fire_lake[i].sprite.getPosition().x, fire_lake[i].sprite.getPosition().y + (fire_lake[i].sprite.getGlobalBounds().height * 2 / 3));
    }

    lakes[2].setPosition(fire_lake[0].sprite.getPosition().x, fire_lake[0].sprite.getPosition().y + (fire_lake[0].sprite.getGlobalBounds().height  / 3.5));

    RenderWindow window = { VideoMode(1920,1080),"sfml works" };
    Event event;
    Clock clock;
    clock.restart();

    RectangleShape mm(Vector2f(150.0f, 30.0f));
	mm.setPosition(1450,850);
	mm.setFillColor(Color::Red);

    character fireboy;
    fireboy.frameHeight = 140;
    Texture boytex;
    if (!boytex.loadFromFile("game_textures\\fireboy_run.png")) {
        cout << "Error: Could not load fireboy image! Check the path";
    }
    fireboy.sprite.setTexture(boytex);
    fireboy.sprite_origin();
    fireboy.sprite.setPosition(500, ground[2].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
    fireboy.framecounter = 0;
    fireboy.totalFrames = 6; //boy 6, girl 9
    fireboy.delay = 0.1f;
    fireboy.speed_x = 450.0f; //450
    fireboy.speed_y = 0.0f;
    fireboy.jump_strength = -900.0f;
    fireboy.gravity = 2500.0f;
    fireboy.timer = fireboy.delay;
    fireboy.onground = false;
    fireboy.ismoving = false;
    fireboy.stop = false;
    fireboy.right = Keyboard::Right;
    fireboy.left = Keyboard::Left;
    fireboy.up = Keyboard::Up;

    character watergirl;
    watergirl.frameHeight = 128;
    Texture girltex;
    if (!girltex.loadFromFile("game_textures\\watergirl_run.png")) {
        cout << "Error: Could not load watergirl image! Check the path";
    }
    watergirl.sprite.setTexture(girltex);
    watergirl.sprite_origin();
    watergirl.sprite.setPosition(400, ground[2].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
    watergirl.framecounter = 0;
    watergirl.totalFrames = 9; //boy 6, girl 9
    watergirl.delay = 0.1f;
    watergirl.speed_x = 450.0f;
    watergirl.speed_y = 0.0f;
    watergirl.jump_strength = -900.0f;
    watergirl.gravity = 2500.0f;
    watergirl.timer = watergirl.delay;
    watergirl.onground = false;
    watergirl.ismoving = false;
    watergirl.stop = false;
    watergirl.right = Keyboard::D;
    watergirl.left = Keyboard::A;
    watergirl.up = Keyboard::W;




    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		fireboy.onground = false;
		watergirl.onground = false;

        for (int i = 3; i < 12; i++) {
            platform_collision(fireboy, collision_boxs[i]);
			platform_collision(watergirl, collision_boxs[i]);
        }
		platform_collision(fireboy, walls[3]);
		platform_collision(watergirl, walls[3]);
		platform_collision(fireboy, mm);
        platform_collision(watergirl, mm);

        for (int i = 1; i < 2; i++) {
            ground_collision(fireboy, collision_boxs[i]);
            ground_collision(watergirl, collision_boxs[i]);
        }
        for (int i = 0; i < 2; i++) {
            water_collision(fireboy, watergirl, lakes[i]);
            fire_collision(fireboy, watergirl, lakes[i+2]);
        }
        fire_collision(fireboy, watergirl, lakes[4]);
        for (int i = 0; i < 3; i++) {
            wall_collision(fireboy, walls[i]);
			wall_collision(watergirl, walls[i]);
        }
		wall_collision(fireboy, collision_boxs[0]);
		wall_collision(watergirl, collision_boxs[0]);

        ground_collision(fireboy, collision_boxs[2]);
        ground_collision(watergirl, collision_boxs[2]);
        ground_collision(fireboy, collision_boxs[1]);
        ground_collision(watergirl, collision_boxs[1]);

		slope_collision(fireboy,collision_boxs[3].getPosition().x - 64, collision_boxs[3].getPosition().y + 64, collision_boxs[3].getPosition().x, collision_boxs[3].getPosition().y);
        slope_collision(fireboy, collision_boxs[8].getPosition().x - 64, collision_boxs[8].getPosition().y + 64, collision_boxs[8].getPosition().x, collision_boxs[8].getPosition().y);
        slope_collision(fireboy, collision_boxs[9].getPosition().x - 64, collision_boxs[9].getPosition().y + 64, collision_boxs[9].getPosition().x, collision_boxs[9].getPosition().y);
        slope_collision(fireboy, collision_boxs[5].getPosition().x - 76, collision_boxs[5].getPosition().y + 128, collision_boxs[5].getPosition().x, collision_boxs[5].getPosition().y);
        slope_collision(fireboy, 1505, 461, 1634, 577);

		collision_boxs[5].setFillColor(Color::Green);

        moveCharacter(fireboy, deltaTime);
        moveCharacter(watergirl, deltaTime);
        jumpCharacter(fireboy, deltaTime);
        jumpCharacter(watergirl, deltaTime);
        animation(fireboy, deltaTime);
        animation(watergirl, deltaTime);


        window.clear();

        window.draw(background.sprite);
        window.draw(Frame.sprite);

        for (int i = 0; i < 12; i++)
            window.draw(ground[i].sprite);

        window.draw(Platform[0].sprite);
        window.draw(Platform[1].sprite);

        for (int i = 0; i < 3; i++)
            window.draw(fire_lake[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(water_lake[i].sprite);
        window.draw(ground[5].sprite);
      //  window.draw(box.sprite);
        window.draw(triangle.sprite);



        for (int i = 0; i < 3; i++)
            window.draw(fire_point[i].sprite);

        for (int i = 0; i < 3; i++)
            window.draw(water_point[i].sprite);
        for (int i = 0; i < 12; i++)
			window.draw(collision_boxs[i]);
		for (int i = 0; i < 4; i++)
			window.draw(walls[i]);
        for (int i = 0; i < 5; i++)
			window.draw(lakes[i]);
        window.draw(fireboy.sprite);
        window.draw(watergirl.sprite);
       // window.draw(mm);
        window.display();
    }
    return 0;
}