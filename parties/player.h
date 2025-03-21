#ifndef PLAYER_H
#define PLAYER_H

struct Player {
    public : float x, y;
    public :float width, height;
    public :float velocityY;
    public :float velocityX;
    public :float depth;
    public :float z;
    public :bool onGround;

    public : Player(float xx,float yy);
    public : void jump();
    public : void update(float deltaTime,int w,bool mr,bool ml);
};

#endif // PLAYER_H