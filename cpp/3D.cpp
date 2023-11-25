#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>


std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<double> dist(0, 1.0);

int WITH  = 2560 ;
int HEIGHT = 1440 ;
int TYPES =  10 ; //siempre hay uno mas de particulas tipo 0 , 1 ,2, 3
int N = 1500 ;

sf::Color hsvToRgb(float hue, float saturation, float value) {
    int hi = static_cast<int>(std::floor(hue / 60.0)) % 6;
    float f = hue / 60.0 - hi;
    float p = value * (1 - saturation);
    float q = value * (1 - f * saturation);
    float t = value * (1 - (1 - f) * saturation);

    switch (hi) {
        case 0: return sf::Color(value * 255, t * 255, p * 255);
        case 1: return sf::Color(q * 255, value * 255, p * 255);
        case 2: return sf::Color(p * 255, value * 255, t * 255);
        case 3: return sf::Color(p * 255, q * 255, value * 255);
        case 4: return sf::Color(t * 255, p * 255, value * 255);
        default: return sf::Color(value * 255, p * 255, q * 255);
    }
}

float map(float value, float start1, float stop1, float start2, float stop2) {
    // Reescala el valor del rango [start1, stop1] al rango [start2, stop2]
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

class Particle {

public:

    float p[3];
    int type;
    float v[3];
    float a[3];
    float radio = 5;
    float drag = 0.5;

    Particle(int type_) {

        this->type = type_;

        float x = static_cast<float>(rand()) / RAND_MAX * 1000 - 500;
        float y = static_cast<float>(rand()) / RAND_MAX * 1000 - 500;
        float z = static_cast<float>(rand()) / RAND_MAX * 1000 - 500;

        this->p[0] = std::fmod(x * dist(rng), x);
        this->p[1] = std::fmod(y * dist(rng), y);
        this->p[2] = std::fmod(z * dist(rng), z);

        this->v[0] = 0.0f;
        this->v[1] = 0.0f;
        this->v[2] = 0.0f;

        this->a[0] = 0.0f;
        this->a[1] = 0.0f;
        this->a[2] = 0.0f;

    }

    void move(){

    this->v[0] += this->a[0];
    this->v[1] += this->a[1];
    this->v[2] += this->a[2];

    this->p[0] += this->v[0];
    this->p[1] += this->v[1];
    this->p[2] += this->v[2];

    this->p[0] = std::fmod(this->p[0], 1000);
    this->p[1] = std::fmod(this->p[1], 1000);
    this->p[2] = std::fmod(this->p[2], 1000);

    this->a[0] *= 0;
    this->a[1] *= 0;
    this->a[2] *= 0;

    this->v[0] *= 0.5;
    this->v[1] *= 0.5;
    this->v[2] *= 0.5;

    }

    void aplayForce(float fx ,float fy,float fz){
      this->a[0] = this->a[0] + fx;
      this->a[1] = this->a[1] + fy;
      this->a[2] = this->a[2] + fz;
    }

    void draw() {
        glPointSize(8.0f);
        float saturation = 1.0f;
        float value = 1.0f;
        sf::Color color = hsvToRgb((360.0f / TYPES) * this->type, saturation, value);
        glColor3f(color.b/255.0f, color.g/255.0f,color.r/255.0f);
        glBegin(GL_POINTS);
        glVertex3f(this->p[0],this->p[1],this->p[2]);
        glEnd();
    }
};

class Interactor {

 private :

   int n ;
   int types ;
   std::vector<Particle> particles ;

 public :

   float A[100][100] ;
   float radio[100][100] ;
   float min[100][100] ;

   Interactor(int types_ , int n_){
      this->n = n_ ;
      this->types = types_;
      this->createParticles();
      this->randomRules();
      this->seeRules();
   }

   void createParticles(){
     for(int i = 0 ; i < this->n ;i++){
       int tipo = std::round(TYPES*dist(rng)) ;
       Particle part( tipo );
       this->particles.push_back(part) ;
     }
   }

   void seeRules(){
     std::cout << "##### RULES ##### Types: " << TYPES << '\n';
     std::cout << "\n";
     std::cout << "----- Matrix A -----" << '\n';
     for(int x = 0 ; x < this->types+1 ; x++ ){
        std::cout << "|";
       for(int y = 0 ; y < this->types+1 ; y++){
        std::cout << this->A[x][y] << ' ';
       }
       std::cout << "|\n";
     }
     std::cout << "\n";
     std::cout << "----- Matrix RADIO -----" << '\n';
     for(int x = 0 ; x < this->types+1 ; x++ ){
        std::cout << "|";
       for(int y = 0 ; y < this->types+1 ; y++){
        std::cout << this->radio[x][y] << ' ';
       }
       std::cout << "|\n";
     }
     std::cout << "\n";
     std::cout << "----- Matrix MIN -----" << '\n';
     for(int x = 0 ; x < this->types+1 ; x++ ){
        std::cout << "|";
       for(int y = 0 ; y < this->types+1 ; y++){
        std::cout << this->min[x][y] << ' ';
       }
       std::cout << "|\n";
     }
      std::cout << "\n";

   }

   void update(){

     for(int i = 0 ; i < N ; i++ ){
       this->particles[i].move();
       this->particles[i].draw();

       float px = this->particles[i].p[0];
       float py = this->particles[i].p[1];
       float pz = this->particles[i].p[2];

       int type = this->particles[i].type;

       float fx = 0;
       float fy = 0;
       float fz = 0;

       for(int j = 0 ; j < N; j++ ){

         float px_oth = this->particles[j].p[0];
         float py_oth = this->particles[j].p[1];
         float pz_oth = this->particles[j].p[2];

         int type_oth = this->particles[j].type;

         if( &this->particles[i] != &this->particles[j] ){

            float sub_x = (px_oth-px);
            float sub_y = (py_oth-py);
            float sub_z = (pz_oth-pz);

            int w = 1000 ;

            if (sub_x > w/2) {
                sub_x -= w;
            }

            if (sub_x < -w/2) {
                sub_x += w;
            }

            if (sub_y > w/2) {
                sub_y -= w;
            }

            if (sub_y < -w/2) {
                sub_y += w;
            }


            if (sub_z > w/2) {
                sub_z -= w;
            }

            if (sub_z < -w) {
                sub_z += w;
            }

            float mag = sqrt(sub_x*sub_x+sub_y*sub_y+sub_z*sub_z);

            float uni_x = sub_x/mag ;
            float uni_y = sub_y/mag ;
            float uni_z = sub_z/mag ;

            float k = 0.125;

            if ( mag < this->min[type][type_oth] ){

                float minForce = fabs(this->A[type][type_oth]) * -2 ;
                float repulse = map(mag,0,this->min[type][type_oth],1,0) ;
                //std::cout << minForce << '\n';
                fx += uni_x * k *repulse *minForce ;
                fy += uni_y * k *repulse *minForce ;
                fz += uni_z * k *repulse *minForce ;
            }

            if ( mag > this->min[type][type_oth] && mag < this->radio[type][type_oth]){

              float maxForce = this->A[type][type_oth] ;
              float ampl = map(mag,0,this->radio[type][type_oth],1,0) ;
              fx += uni_x * ampl* k * maxForce ;
              fy += uni_y * ampl* k * maxForce ;
              fz += uni_z * ampl* k * maxForce ;

            }

         }

       }

        this->particles[i].aplayForce(fx,fy,fz);
     }

   }

   void randomRules(){

     for(int x = 0 ; x < this->types+1; x++ ){
       for(int y = 0 ; y < this->types+1 ; y++){

         std::uniform_real_distribution<double> real_dist(0.3, 1.0);
         double random_real = real_dist(rng);
         if (rng() % 2 == 0) {
             random_real = -random_real;
         }
         this->A[x][y] = static_cast<float>(random_real);

         std::uniform_int_distribution<int> int_dist(70, 250); // Cambia de float a int
         int r = int_dist(rng); // Cambia de float a int
         this->radio[x][y] = static_cast<float>(r);

         std::uniform_int_distribution<int> int_dist2(30, 50); // Cambia de float a int
         int m = int_dist2(rng); // Cambia de float a int
         this->min[x][y] = static_cast<float>(m);
       }
     }

   }



};


float cameraDistance = 5.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
bool isMousePressed = false;
sf::Vector2i lastMousePosition;
std::vector<sf::Vector3f> points;
float pointSize = 15.0f;
std::vector<Particle> particles;

int main() {

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(WITH, HEIGHT), "Life Particles",sf::Style::Fullscreen);
    //sf::RenderWindow window(sf::VideoMode(WITH, HEIGHT), "Life Particles");
    window.setMouseCursorVisible(false);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));



     Interactor inte(TYPES,N);


    srand(static_cast<unsigned int>(time(nullptr)));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseMoved) {
                // Captura el movimiento del ratón para ajustar la cámara
                if (isMousePressed) {
                    sf::Vector2i currentMousePosition = sf::Mouse::getPosition();
                    sf::Vector2i delta = currentMousePosition - lastMousePosition;

                    cameraAngleX += static_cast<float>(delta.y) * 0.1f; // Controla el ángulo de cámara en el eje X
                    cameraAngleY += static_cast<float>(delta.x) * 0.1f; // Controla el ángulo de cámara en el eje Y

                    lastMousePosition = currentMousePosition;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = true;
                    lastMousePosition = sf::Mouse::getPosition();
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = false;
                }
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                // Manejar el zoom de la cámara con la rueda del ratón
                float zoomDelta = event.mouseWheelScroll.delta;
                cameraDistance += zoomDelta * 55.0f;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 10000.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -cameraDistance - 1000.0f);
        glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
        glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

        inte.update();

        window.display();
    }

    return 0;
}
