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

    int framecounter = 0;
	float timer = 0;
	bool opened = false;
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
    hitbox.left += 65;
    if (hitbox.intersects(ground.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = ground.getPosition().y;
        if (player.speed_y > 0 && playerBottom <= platformTop + 30.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, platformTop - (player.frameHeight / 2.0));
            player.speed_y = 0;
            player.onground = true;
        }
        if (player.speed_y < 0 && hitbox.top >= platformTop - 30.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, platformTop + (ground.getSize().y) + (player.frameHeight / 2.0));
            player.speed_y = 0;
        }
    }
}

bool girl_is_dead = 0;
int water_framecounter = 0;
void water_died(character& player, design& smoke, float& dt) {
    player.stop = 1;
    girl_is_dead = 1;
    player.timer += dt;
    smoke.sprite.setPosition(player.sprite.getPosition().x - 84, player.sprite.getPosition().y - 150);
    if (girl_is_dead && player.timer >= 0.1f) {
        smoke.sprite.setTextureRect(IntRect(water_framecounter * 168, 0, 168, 186));
        player.timer = 0;
        water_framecounter++;
    }
    if (water_framecounter >= 5) {
        smoke.sprite.setTextureRect(IntRect(5 * 168, 0, 168, 186));
    }
    if (player.stop) {
        player.speed_x = 0;
    }
    else {
        player.speed_x = 450.0f;
    }
}

bool boy_is_dead = 0;
int fire_framecounter = 0;
void fire_died(character& player, design& smoke, float& dt) {
	player.stop = 1;
    boy_is_dead = 1;
    player.timer += dt;
    smoke.sprite.setPosition(player.sprite.getPosition().x - 84, player.sprite.getPosition().y - 150);
    if (boy_is_dead && player.timer >= 0.1f) {
        smoke.sprite.setTextureRect(IntRect(fire_framecounter * 168, 0, 168, 186));
        player.timer = 0;
        fire_framecounter++;
    }
    if (fire_framecounter >= 5) {
        smoke.sprite.setTextureRect(IntRect(5 * 168, 0, 168, 186));
    }
    if (player.stop) {
        player.speed_x = 0;
    }
    else {
        player.speed_x = 450.0f;
    }
}

void fire_collision(character& fireBoy, character& waterGirl, RectangleShape& fire, design& smoke, float& dt) {
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
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop - (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
            fireBoy.onground = true;
        }
        if (fireBoy.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop + (fire.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
    if (hitbox2.intersects(fire.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = fire.getPosition().y;
        if (waterGirl.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            water_died(waterGirl, smoke, dt);
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop2 - (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
            waterGirl.onground = true;

        }
        if (waterGirl.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop2 + (fire.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
}

void water_collision(character& fireBoy, character& waterGirl, RectangleShape& water, design& smoke, float& dt) {
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
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop - (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
            waterGirl.onground = true;
        }
        if (waterGirl.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop + (water.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
    if (hitbox2.intersects(water.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = water.getPosition().y;
        if (fireBoy.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            fire_died(fireBoy, smoke, dt);
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 - (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
            fireBoy.onground = true;
		
        }
        if (fireBoy.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 + (water.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
}
void acid_collision(character& fireBoy, character& waterGirl, RectangleShape& acid, design& smoke, float& dt) {
    FloatRect hitbox = waterGirl.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = fireBoy.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(acid.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = acid.getPosition().y;
        if (waterGirl.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            water_died(waterGirl, smoke, dt);
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop - (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
            waterGirl.onground = true;
        }
        if (waterGirl.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop + (acid.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
    if (hitbox2.intersects(acid.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = acid.getPosition().y;
        if (fireBoy.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            fire_died(fireBoy, smoke, dt);
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 - (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
            fireBoy.onground = true;

        }
        if (fireBoy.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 + (acid.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
}



void slope_collision(character& player, double x1, double y1, double x2, double y2) {
    double player_x = player.sprite.getPosition().x;
    if (player_x >= x1 && player_x <= x2) {
        double slopeY = y1 + (player_x - x1) * ((y2 - y1) / (x2 - x1));
        double player_y = player.sprite.getPosition().y + (player.frameHeight / 2.0);
        if ((player_y >= slopeY - 5.0) && (player_y <= slopeY + 15.0)) {
            if (player.speed_y >= 0) {
                player.sprite.setPosition(player_x, slopeY - (player.frameHeight / 2.0f));
                player.speed_y = 0;
                player.onground = true;
            }
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



void fire_door_collision(character& player, design& door, float dt) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
	door.opened = hitbox.intersects(door.sprite.getGlobalBounds());
    door.timer += dt;
    if (door.timer >= 0.16f) {
        door.timer = 0;
        if (door.opened) {
            if (door.framecounter < 6) {
                door.framecounter++;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
        else {
            if (door.framecounter > 0) {
                door.framecounter--;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
    }
}



void water_door_collision(character& player, design& door, float dt) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
	door.opened = hitbox.intersects(door.sprite.getGlobalBounds());
    door.timer += dt;
    if (door.timer >= 0.16f) {
        door.timer = 0;
        if (door.opened) {
            if (door.framecounter < 6) {
                door.framecounter++;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
        else {
            if (door.framecounter > 0) {
                door.framecounter--;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
    }
}


int points_counter = 0;
void point_collision(character& player1, character& player2, design& point) {
    FloatRect hitbox = player1.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = player2.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(point.sprite.getGlobalBounds())) {
        point.sprite.setColor(Color(255, 255, 255, 0));
		points_counter++;
    }
    else if(hitbox2.intersects(point.sprite.getGlobalBounds())) {
		point.sprite.setColor(Color(255, 255, 255, 0));
    }
}




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
    design smoke;
    design door[2];
    RectangleShape rec[135];
	RectangleShape collision_boxs2[16];    
	RectangleShape lakes2[6];



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
// Level 2 definitions
    design background2;

    design ground2[10];
    design platform2[2];

    design fire_lake2[2];
    design water_lake2[2];
    design green_lake2[2];

    design fire_point2[8];
    design water_point2[8];

    design slope_left2;
    design slope_right2;
    design right_corner2;
    design step_corner2;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load level1
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    smoke.texture.loadFromFile("game_textures\\smoke2.0.png");
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
    door[0].texture.loadFromFile("game_textures\\fireboyenddoor.png");
    door[1].texture.loadFromFile("game_textures\\watergirlenddoor.png");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    // 
// Load level 2 textures

    background2.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");

    for (int i = 0; i < 10; i++)
        ground2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/Tile_56_1_1.png");

    for (int i = 0; i < 2; i++)
        platform2[i].texture.loadFromFile("game_textures/Platform/sprite_platform2.00.png");

    for (int i = 0; i < 2; i++)
        fire_lake2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite_0_1.png");

    for (int i = 0; i < 2; i++)
        water_lake2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");

    for (int i = 0; i < 2; i++)
        green_lake2[i].texture.loadFromFile("acid_lake/208910c6-0852-4eec-bdad-457062911bb6.png");

    for (int i = 0; i < 8; i++)
        fire_point2[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Icons_Fire Spell.png");

    for (int i = 0; i < 8; i++)
        water_point2[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Water_Spell_Frame_03.png");

    slope_left2.texture.loadFromFile("leveel2/Tile_48_1.png");   
    slope_right2.texture.loadFromFile("leveel2/Tile_48_2.png");  
    right_corner2.texture.loadFromFile("leveel2/Tile_48_3.png");
    
        step_corner2.texture.loadFromFile("leveel2/Tile_48_4.png"); 

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    smoke.sprite.setTexture(smoke.texture);
    smoke.sprite.setTextureRect(IntRect(0, 0, 168, 186));

    for (int i = 0; i < 2; i++) {
        door[i].sprite.setTexture(door[i].texture);
        door[i].sprite.setTextureRect(IntRect(0, 0, 121, 144));
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
// sprites for level 2

    background2.sprite.setTexture(background2.texture);

    for (int i = 0; i < 10; i++)
        ground2[i].sprite.setTexture(ground2[i].texture);

    for (int i = 0; i < 2; i++)
        platform2[i].sprite.setTexture(platform2[i].texture);

    for (int i = 0; i < 2; i++)
        fire_lake2[i].sprite.setTexture(fire_lake2[i].texture);

    for (int i = 0; i < 2; i++)
        water_lake2[i].sprite.setTexture(water_lake2[i].texture);

    for (int i = 0; i < 2; i++)
        green_lake2[i].sprite.setTexture(green_lake2[i].texture);

    for (int i = 0; i < 8; i++)
        fire_point2[i].sprite.setTexture(fire_point2[i].texture);

    for (int i = 0; i < 8; i++)
        water_point2[i].sprite.setTexture(water_point2[i].texture);

    slope_right2.sprite.setTexture(slope_right2.texture);
    right_corner2.sprite.setTexture(right_corner2.texture);

    
        step_corner2.sprite.setTexture(step_corner2.texture);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < 135; i++) {
		rec[i].setSize(Vector2f(4+2*i, 1));
        rec[i].setPosition(1504 - i, 470 + i);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Credit Window
    Texture cr_background;
    cr_background.loadFromFile("game_textures\\resized_1920x1080.png");
    Sprite credit_BG;
    credit_BG.setTexture(cr_background);
    
    Font font;
    if (!font.loadFromFile("game_textures\\Copy_of_font.TTF")) {
     
        return -1;
    }
    Text namesarr[8];
    string names_arr[] = { "Nourhan Ashraf","Malak Ahmed","Nour Gad","Aya Mohamed","Mohamed Mahmoud","Ziad Ekramy","Ziad Tarek" ,"To return Press ( X )"};
    for (int i = 0; i < 8; i++) {
        namesarr[i].setString(names_arr[i]);
        namesarr[i].setCharacterSize(27);
        namesarr[i].setFillColor(Color::White);
        namesarr[i].setOutlineThickness(4);
        namesarr[i].setOutlineColor(Color::Green);
        namesarr[i].setPosition(800, 150 + i * 100);
        namesarr[i].setScale(2.0f, 2.0f);
        namesarr[i].setFont(font);
        }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //pause menu
    Texture mn_menu;
    mn_menu.loadFromFile("game_textures\\main_menu_resized_1920x1080.png");
    Sprite menu_BG;
    menu_BG.setTexture(mn_menu);

    Text pause_txt[3];
    String pausetxt[] = { "Resume","Back to main menu","Back to levels menu"};
    for (int i = 0; i < 3; i++) {
        pause_txt[i].setString(pausetxt[i]);
        pause_txt[i].setCharacterSize(27);
        pause_txt[i].setFillColor(Color::White);
        pause_txt[i].setOutlineThickness(4);
        pause_txt[i].setOutlineColor(Color::Red);
       if(i>0) pause_txt[i].setPosition(740, 300 + i * 200);
       else  pause_txt[i].setPosition(890, 300 + i * 200);
        pause_txt[i].setScale(2.5f, 2.5f);
        pause_txt[i].setFont(font);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //control menu
    Texture control_BG,F_boy_BG,W_Girl_BG;
    control_BG.loadFromFile("game_textures\\resized_1920x1080.jpg");
    F_boy_BG.loadFromFile("game_textures\\ChatGPT_f_controlsImage.png");
    W_Girl_BG.loadFromFile("game_textures\\ChatGPT_W_controlsImage.png");
    Sprite c_BG, F_BG, W_BG;
    c_BG.setTexture(control_BG);
    F_BG.setTexture(F_boy_BG);
    W_BG.setTexture(W_Girl_BG);

    W_BG.setScale(0.4f, 0.4f);
    F_BG.setScale(0.4f, 0.4f);

    W_BG.setPosition(205, 440);
    F_BG.setPosition(1135, 440);

    Text control_txt[2];
    String controltxt[] = { "Fire Boy Controls","Water Girl Controls" };
    for (int i = 0; i < 2; i++) {
        control_txt[i].setString(controltxt[i]);
        control_txt[i].setCharacterSize(27);
        control_txt[i].setFillColor(Color::Black);
        control_txt[i].setOutlineThickness(4);
        control_txt[i].setOutlineColor(Color::White);
        control_txt[i].setPosition(280 + i * 920, 350);
        control_txt[i].setScale(2.5f, 2.5f);
        control_txt[i].setFont(font);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);

    //fire door		
    door[0].sprite.setPosition(50, 856 + 10 + 2);

    //water door
    door[1].sprite.setPosition(296, 1000 - 691.2 + 100 - 80 + 10 - 144 + 2);


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
    ground[3].sprite.setPosition(1920 - 105.6 - 182, 700+10 );


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
    ground[8].sprite.setPosition(1920 - 105.6 - 182, 450-90 );


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
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Level 2 Design
// ==========================
    background2.sprite.setScale(1.875f, 1.5f);

    // frame
    Frame.sprite.setScale(1.1f, 1.5f);
    Frame.sprite.setPosition(-96, -128);

    // --------------------------
    // Ground
    // --------------------------

    // top left ground
    ground2[0].sprite.setScale(3.15f, 1.0f);
    ground2[0].sprite.setPosition(17, 185);

    // top right ground
    ground2[1].sprite.setScale(1.65f, 1.0f);
    ground2[1].sprite.setPosition(1116, 185);

    // left vertical block
    ground2[2].sprite.setScale(0.65f, 1.95f);
    ground2[2].sprite.setPosition(17, 600);

    // middle ground carrying green lakes
    ground2[3].sprite.setScale(0.59375f, 1.0f);
    ground2[3].sprite.setPosition(260, 470);

    ground2[8].sprite.setScale(1.3f, 1.0f);
    ground2[8].sprite.setPosition(850, 470);

    ground2[9].sprite.setScale(0.75f, 1.0f);
    ground2[9].sprite.setPosition(1670, 470);


    // long lower-middle ground
    ground2[4].sprite.setScale(5.3f, 0.90f);
    ground2[4].sprite.setPosition(15, 700);

    // bottom ground
    ground2[5].sprite.setScale(1.16f, 1.0f);
    ground2[5].sprite.setPosition(0, 1016);

    ground2[6].sprite.setScale(1.6f, 1.0f);
    ground2[6].sprite.setPosition(666.64, 1016);

  
    ground2[7].sprite.setScale(1.05f, 1.0f);
    ground2[7].sprite.setPosition(1465, 1016);

    // --------------------------
    // platforms
    // --------------------------
    platform2[0].sprite.setPosition(550, 365);
    platform2[0].sprite.setScale(1.08f, 1.0f);

    platform2[1].sprite.setPosition(1325, 365);
    platform2[1].sprite.setScale(1.08f, 1.0f);

    // --------------------------
    // Corners
    // --------------------------

    // top-right sloped piece
    slope_right2.sprite.setPosition(1830, 355);
    slope_right2.sprite.setScale(0.50f, 0.70f);

    // upper-right step
    step_corner2.sprite.setPosition(1630, 183);
    step_corner2.sprite.setScale(1.08f, 1.50f);

    // middle-right vertical corner
    right_corner2.sprite.setPosition(1770, 850);
    right_corner2.sprite.setScale(5.9f, 10.9f);
    

    // --------------------------
    // Lakes
    // --------------------------

    // green lakes in the middle
    green_lake2[0].sprite.setScale(1.42f, 2.0f);
    green_lake2[0].sprite.setPosition(450, 470);

    green_lake2[1].sprite.setScale(1.42f, 2.0f);
    green_lake2[1].sprite.setPosition(1260, 470);

    // left lower water lake
    water_lake2[0].sprite.setScale(1.26f, 1.0f);
    water_lake2[0].sprite.setPosition(320, 878);

    // right lower fire lake
    fire_lake2[0].sprite.setScale(1.26f, 1.0f);
    fire_lake2[0].sprite.setPosition(1110, 878);

    // bottom left fire lake
    fire_lake2[1].sprite.setScale(1.03f, 2.0f);
    fire_lake2[1].sprite.setPosition(370, 1016);

    // bottom right water lake
    water_lake2[1].sprite.setScale(1.03f, 2.0f);
    water_lake2[1].sprite.setPosition(1170, 1016);

    // --------------------------
    // Points scale
    // --------------------------
    for (int i = 0; i < 8; i++)
    {
        fire_point2[i].sprite.setScale(0.1f, 0.1f);
        water_point2[i].sprite.setScale(0.1f, 0.1f);
    }

    // --------------------------
    // Top pair
    // --------------------------
    fire_point2[0].sprite.setPosition(1000, 150);
    water_point2[0].sprite.setPosition(1070, 150);

    // --------------------------
    // Middle pair above green lakes
    // --------------------------
    fire_point2[1].sprite.setPosition(1010, 430);
    water_point2[1].sprite.setPosition(1100, 430);

    // --------------------------
    // Lower-middle row
    // --------------------------
    water_point2[2].sprite.setPosition(490, 640);
    fire_point2[2].sprite.setPosition(560, 640);

    water_point2[3].sprite.setPosition(1180, 640);
    fire_point2[3].sprite.setPosition(1250, 640);

    // --------------------------
    // Above lower lakes
    // --------------------------
    water_point2[4].sprite.setPosition(430, 840);
    water_point2[5].sprite.setPosition(520, 840);

    fire_point2[4].sprite.setPosition(1210, 840);
    fire_point2[5].sprite.setPosition(1320, 840);

    // --------------------------
    // Near bottom lakes
    // --------------------------
    fire_point2[6].sprite.setPosition(470, 985);
    fire_point2[7].sprite.setPosition(520, 985);

    water_point2[6].sprite.setPosition(1270, 985);
    water_point2[7].sprite.setPosition(1320, 985);

    ////////////////////////////////////////////////////////////////////////////////////////////////////


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
    collision_boxs[7].setSize(Vector2f(ground[7].sprite.getGlobalBounds().width + 92 , ground[7].sprite.getGlobalBounds().height));
    collision_boxs[7].setPosition(ground[7].sprite.getPosition().x - 92 , ground[7].sprite.getPosition().y);
    collision_boxs[7].setFillColor(Color::Red);
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

    for (int i = 0; i < 10; i++) {
        Vector2f vect(ground2[i].sprite.getGlobalBounds().width, ground2[i].sprite.getGlobalBounds().height);
		collision_boxs2[i].setSize(vect);
		collision_boxs2[i].setPosition(ground2[i].sprite.getPosition());
    }
    collision_boxs2[10].setSize(Vector2f(platform2[0].sprite.getGlobalBounds().width, platform2[0].sprite.getGlobalBounds().height));
    collision_boxs2[10].setPosition(platform2[0].sprite.getPosition());
    collision_boxs2[11].setSize(Vector2f(platform2[1].sprite.getGlobalBounds().width, platform2[1].sprite.getGlobalBounds().height));
    collision_boxs2[11].setPosition(platform2[1].sprite.getPosition());
	collision_boxs2[12].setSize(Vector2f(slope_right2.sprite.getGlobalBounds().width, slope_right2.sprite.getGlobalBounds().height-64));
	collision_boxs2[12].setPosition(slope_right2.sprite.getPosition().x,slope_right2.sprite.getPosition().y+64);
	collision_boxs2[13].setSize(Vector2f(right_corner2.sprite.getGlobalBounds().width, right_corner2.sprite.getGlobalBounds().height));
	collision_boxs2[13].setPosition(right_corner2.sprite.getPosition());
	collision_boxs2[14].setSize(Vector2f(step_corner2.sprite.getGlobalBounds().width, step_corner2.sprite.getGlobalBounds().height-96));
	collision_boxs2[14].setPosition(step_corner2.sprite.getPosition().x, step_corner2.sprite.getPosition().y + 100-2);
    collision_boxs2[12].setFillColor(Color::Blue);
    collision_boxs2[1].setSize(Vector2f(ground2[1].sprite.getGlobalBounds().width+55, ground2[1].sprite.getGlobalBounds().height));
    collision_boxs2[15].setSize(Vector2f(slope_right2.sprite.getGlobalBounds().width-64+32, slope_right2.sprite.getGlobalBounds().height));
    collision_boxs2[15].setPosition(slope_right2.sprite.getPosition().x+64-32, slope_right2.sprite.getPosition().y);

    for (int i = 0; i < 2; i++) {
        Vector2f vect(water_lake2[i].sprite.getGlobalBounds().width, water_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i].setSize(vect);
        lakes2[i].setPosition(water_lake2[i].sprite.getPosition().x, water_lake2[i].sprite.getPosition().y + (water_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
        Vector2f vect2(green_lake2[i].sprite.getGlobalBounds().width, green_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i+2].setSize(vect2);
        lakes2[i+2].setPosition(green_lake2[i].sprite.getPosition().x, green_lake2[i].sprite.getPosition().y + (green_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
        Vector2f vect3(fire_lake2[i].sprite.getGlobalBounds().width, fire_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i+4].setSize(vect3);
        lakes2[i+4].setPosition(fire_lake2[i].sprite.getPosition().x, fire_lake2[i].sprite.getPosition().y + (fire_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
    }


    RenderWindow window = { VideoMode(1920,1080),"sfml works" };
    Event event;
    Clock clock;
    clock.restart();

    RectangleShape platform(Vector2f(150.0f, 30.0f));
	platform.setPosition(1450,850);

    character fireboy;
    fireboy.frameHeight = 140;
    Texture boytex;
    if (!boytex.loadFromFile("game_textures\\fireboy_run.png")) {
        cout << "Error: Could not load fireboy image! Check the path";
    }
    fireboy.sprite.setTexture(boytex);
    fireboy.sprite_origin(); //500                                                                //-300 xx
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
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
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
        //platform_collision(fireboy, walls[2]);
        //platform_collision(watergirl, walls[2]);
        platform_collision(fireboy, platform);
        platform_collision(watergirl, platform);

        for (int i = 1; i <= 2; i++) {
            ground_collision(fireboy, collision_boxs[i]);
            ground_collision(watergirl, collision_boxs[i]);
        }
        for (int i = 0; i < 2; i++) {
            water_collision(fireboy, watergirl, lakes[i],smoke,deltaTime);
            fire_collision(fireboy, watergirl, lakes[i+2], smoke,deltaTime);
        }
        fire_collision(fireboy, watergirl, lakes[4], smoke,deltaTime);
        for (int i = 0; i < 2; i++) {
            wall_collision(fireboy, walls[i]);
			wall_collision(watergirl, walls[i]);
        }
		wall_collision(fireboy, collision_boxs[0]);
		wall_collision(watergirl, collision_boxs[0]);

        ground_collision(fireboy, collision_boxs[2]);
        ground_collision(watergirl, collision_boxs[2]);
        ground_collision(fireboy, collision_boxs[1]);
        ground_collision(watergirl, collision_boxs[1]);

        slope_collision(fireboy, collision_boxs[3].getPosition().x - 64, collision_boxs[3].getPosition().y + 64, collision_boxs[3].getPosition().x, collision_boxs[3].getPosition().y);
        slope_collision(fireboy, collision_boxs[8].getPosition().x - 64, collision_boxs[8].getPosition().y + 64, collision_boxs[8].getPosition().x, collision_boxs[8].getPosition().y);
        slope_collision(fireboy, collision_boxs[9].getPosition().x - 64, collision_boxs[9].getPosition().y + 64, collision_boxs[9].getPosition().x, collision_boxs[9].getPosition().y);
        slope_collision(fireboy, 627, 655, 692, 559);
        slope_collision(fireboy, 1505, 470, 1645, 580); //triangle
        slope_collision(fireboy, 1365, 580, 1505, 470); //tringle
        slope_collision(fireboy, 700, 987, 764, 1045); // fire lake 2
        slope_collision(fireboy, 1165, 1045, 1218, 987); //fire lake 2
        slope_collision(fireboy, 1258, 655, 1274, 617); //fire lake 3
        slope_collision(fireboy, 1038, 559, 1104, 657); //fire lake 3
        slope_collision(fireboy, 456, 618, 475, 656); //water lake 0
        slope_collision(fireboy, 1502, 227, 1575, 289); //ground 11
        slope_collision(fireboy, 1275, 270, 1295, 230); //fire lake 4
        slope_collision(fireboy, 1090, 230, 1111, 270); //fire lake 4
        slope_collision(fireboy, 730, 230, 750, 270); //water lake 1
        slope_collision(fireboy, 913, 270, 933, 230); //water lake 1

        slope_collision(watergirl, collision_boxs[3].getPosition().x - 64, collision_boxs[3].getPosition().y + 64, collision_boxs[3].getPosition().x + 10, collision_boxs[3].getPosition().y - 10);
        slope_collision(watergirl, collision_boxs[8].getPosition().x - 64, collision_boxs[8].getPosition().y + 64, collision_boxs[8].getPosition().x, collision_boxs[8].getPosition().y);
        slope_collision(watergirl, collision_boxs[9].getPosition().x - 64, collision_boxs[9].getPosition().y + 64, collision_boxs[9].getPosition().x, collision_boxs[9].getPosition().y);
        slope_collision(watergirl, 627, 655, 692, 559);
        slope_collision(watergirl, 1505, 480, 1645, 570); //triangle
        slope_collision(watergirl, 1365, 570, 1505, 480); //tringle
        slope_collision(watergirl, 700, 987, 764, 1045); // fire lake 2
        slope_collision(watergirl, 1165, 1045, 1218, 987); //fire lake 2
        slope_collision(watergirl, 1258, 655, 1274, 617); //fire lake 3
        slope_collision(watergirl, 1038, 559, 1104, 657); //fire lake 3
        slope_collision(watergirl, 456, 618, 475, 656); //water lake 0
        slope_collision(watergirl, 1502, 227, 1575, 289); //ground 11
        slope_collision(watergirl, 1275, 270, 1295, 230); //fire lake 4
        slope_collision(watergirl, 1090, 230, 1110, 270); //fire lake 4
        slope_collision(watergirl, 730, 230, 750, 270); //water lake 1
        slope_collision(watergirl, 913, 270, 933, 230); //water lake 1

        fire_door_collision(fireboy, door[0], deltaTime);
        water_door_collision(watergirl, door[1], deltaTime);

        if (door[0].opened && door[1].opened) {  //door[0].framecounter >= 6 && door[1].framecounter >= 6
			static float timer = 0;
			timer += deltaTime;
            if (timer >= 0.16f) {
                timer = 0.0f;
                if ((door[0].framecounter >= 6 && door[0].framecounter < 17) && (door[1].framecounter >= 6 && door[1].framecounter < 17)) {
                    door[0].framecounter++;
                    door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                    door[1].framecounter++;
                    door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                    watergirl.sprite.setColor(Color::Transparent);
                    watergirl.speed_x = 0;
                    watergirl.speed_y = 0;
                    fireboy.sprite.setColor(Color::Transparent);
                    fireboy.speed_x = 0;
                    fireboy.speed_y = 0;
                    if (door[0].framecounter >= 17 || door[1].framecounter >= 17) {
                        door[0].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                        door[1].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                    }
                }
            }
        }
        else {
            if (door[0].opened && door[0].framecounter >= 6 && !door[1].opened) {
                door[0].framecounter = 6;
                door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
            }
            if (door[1].opened && door[1].framecounter >= 6 && !door[0].opened) {
                door[1].framecounter = 6;
                door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
            }
        }
        

        for(int i=0;i<3;i++)
		    point_collision(fireboy,watergirl ,fire_point[i]);
        for (int i = 0; i < 3; i++)
			point_collision(watergirl,fireboy ,water_point[i]);

        moveCharacter(fireboy, deltaTime);
        moveCharacter(watergirl, deltaTime);
        jumpCharacter(fireboy, deltaTime);
        jumpCharacter(watergirl, deltaTime);
        animation(fireboy, deltaTime);
        animation(watergirl, deltaTime);

        for (int i = 0; i < 135; i++) {
			platform_collision(fireboy, rec[i]);
			platform_collision(watergirl, rec[i]);
        }


        window.clear();

		//level(1) draw 
/*        for (int i = 0; i < 12; i++)
			window.draw(collision_boxs[i]);
		for (int i = 0; i < 4; i++)
			window.draw(walls[i]);
        for (int i = 0; i < 5; i++)
			window.draw(lakes[i]);

        window.draw(background.sprite);
        window.draw(Frame.sprite);

        window.draw(door[0].sprite);
        window.draw(door[1].sprite);


        if (!boy_is_dead)
            window.draw(fireboy.sprite);
        else {
            window.draw(smoke.sprite);
        }

        if (!girl_is_dead)
            window.draw(watergirl.sprite);
        else {
            window.draw(smoke.sprite);
        }


        for (int i = 0; i < 12; i++)
            window.draw(ground[i].sprite);

        window.draw(Platform[0].sprite);
        window.draw(Platform[1].sprite);

        for (int i = 0; i < 3; i++)
            window.draw(fire_lake[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(water_lake[i].sprite);

        window.draw(ground[5].sprite);
        window.draw(triangle.sprite);



        for (int i = 0; i < 3; i++)
            window.draw(fire_point[i].sprite);

        for (int i = 0; i < 3; i++)
            window.draw(water_point[i].sprite);        
         */    

        //level(2)draw 

        window.draw(background2.sprite);

        /////////////////////////

        for (int i = 0; i < 10; i++)
            window.draw(ground2[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(platform2[i].sprite);


        window.draw(slope_right2.sprite);
        window.draw(right_corner2.sprite);

        
            window.draw(step_corner2.sprite);

        for (int i = 0; i < 2; i++)
            window.draw(fire_lake2[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(water_lake2[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(green_lake2[i].sprite);

        for (int i = 0; i < 8; i++)
            window.draw(fire_point2[i].sprite);

        for (int i = 0; i < 8; i++)
            window.draw(water_point2[i].sprite);

        for(int i=0;i<16;i++)
			window.draw(collision_boxs2[i]);
        for (int i = 0; i < 6; i++) 
			window.draw(lakes2[i]);
        

        window.draw(Frame.sprite);

        //DRAWING CREDIT MENU
        /*window.draw(credit_BG);
        for (int i = 0; i < 8; i++) {

            window.draw(namesarr[i]);
        }*/

        //Drawing pause menu
        /*window.draw(menu_BG);
        for (int i = 0; i < 3; i++) {
            window.draw(pause_txt[i]);
        }*/

        //Drawing control menu
        /*window.draw(c_BG);
        window.draw(F_BG);
        window.draw(W_BG);
        
        for (int  i = 0; i < 2; i++) {
            window.draw(control_txt[i]);
        }*/
    


 
        window.display();
    }
    return 0;
}
