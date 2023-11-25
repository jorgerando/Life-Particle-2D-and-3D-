#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <cmath>

int WITH  = 2560 ;
int HEIGHT = 1440 ;
#define TYPES 10 //siempre hay uno mas de particulas tipo 0 , 1 , 3 2560x1440
#define N 1500

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

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

private:

public:
    float p[2];
    int type;
    float v[2];
    float a[2];
    float radio = 3;
    float drag = 0.5;
    sf::CircleShape circle;

    Particle(int type_) {
        this->type = type_;

        this->p[0] = std::fmod(WITH * dist(rng), WITH);
        this->p[1] = std::fmod(HEIGHT * dist(rng), HEIGHT);

        this->v[0] = 0.0f;
        this->v[1] = 0.0f;

        this->a[0] = 0.0f;
        this->a[1] = 0.0f;

        float saturation = 1.0f;
        float value = 1.0f;

        sf::Color color = hsvToRgb((360/TYPES+1)*this->type, saturation, value);
        this->circle.setFillColor(color);
        this->circle.setPosition(this->p[0], this->p[1]);
        this->circle.setRadius(this->radio);
    }

    void move(){
      // Actualiza la velocidad usando la aceleración
          this->v[0] += this->a[0];
          this->v[1] += this->a[1];

          // Actualiza la posición usando la velocidad
          this->p[0] += this->v[0];
          this->p[1] += this->v[1];

    // Aplica límites periódicos (asegurando que las partículas permanezcan dentro de la ventana)
    this->p[0] = std::fmod(this->p[0], WITH);
    this->p[1] = std::fmod(this->p[1], HEIGHT);

    // Asegura que la posición sea no negativa
    if (this->p[0] < 0) {
        this->p[0] += WITH;
    }
    if (this->p[1] < 0) {
        this->p[1] += HEIGHT;
    }

    // Reduzca la aceleración (esto simula una especie de fricción)
    this->a[0] *= 0;
    this->a[1] *= 0;


    // Reduzca la velocidad (esto simula una especie de fricción)
    this->v[0] *= 0.5;
    this->v[1] *= 0.5;

    }

    void aplayForce(float fx ,float fy){
      this->a[0] = this->a[0] + fx;
      this->a[1] = this->a[1] + fy;

    }

    void draw(sf::RenderWindow& window) {
        this->circle.setPosition(this->p[0], this->p[1]);
        window.draw(this->circle);
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
       int tipo = std::round(this->types*dist(rng)) ;
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

   void update(sf::RenderWindow& window){

     for(int i = 0 ; i < this->n ; i++ ){
       this->particles[i].move();
       this->particles[i].draw(window);

       float px = this->particles[i].p[0];
       float py = this->particles[i].p[1];
       int type = this->particles[i].type; ;

       float fx = 0;
       float fy = 0;

       for(int j = 0 ; j < this->n; j++ ){

         float px_oth = this->particles[j].p[0];
         float py_oth = this->particles[j].p[1];
         int type_oth = this->particles[j].type;

         if( &this->particles[i] != &this->particles[j] ){

            float sub_x = (px_oth-px);
            float sub_y = (py_oth-py);

            if (sub_x > WITH/2) {
                sub_x -= WITH;
            }

            if (sub_x < -WITH/2) {
                sub_x += WITH;
            }

            if (sub_y > HEIGHT/2) {
                sub_y -= HEIGHT;
            }

            if (sub_y < -HEIGHT/2) {
                sub_y += HEIGHT;
            }

            float mag = sqrt(sub_x*sub_x+sub_y*sub_y);

            float uni_x = sub_x/mag ;
            float uni_y = sub_y/mag ;

            //float k = 0.35;
            float k = 0.4;

            if ( mag < this->min[type][type_oth] ){

                float minForce = fabs(this->A[type][type_oth]) * -2.5 ;
                float repulse = map(mag,0,this->min[type][type_oth],1,0) ;
                //std::cout << minForce << '\n';
                fx += uni_x * k *repulse *minForce ;
                fy += uni_y * k *repulse *minForce ;
            }

            if ( mag > this->min[type][type_oth] && mag < this->radio[type][type_oth]){

              float maxForce = this->A[type][type_oth] ;
              float ampl = map(mag,0,this->radio[type][type_oth],1,0) ;
              fx += uni_x * ampl* k * maxForce ;
              fy += uni_y * ampl* k * maxForce ;

            }

         }

       }

        this->particles[i].aplayForce(fx,fy);
     }

   }

   void randomRules(){

     for(int x = 0 ; x < this->types+1 ; x++ ){
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

int main() {

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    //sf::RenderWindow window(sf::VideoMode(WITH, HEIGHT), "Life Particles",sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(WITH, HEIGHT), "Life Particles");
    window.setFramerateLimit(60);

   Interactor inter(TYPES,N) ;
   Particle  par(0) ;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(); // Limpia el contenido de la ventana

        par.draw(window);
        inter.update(window);

        window.display(); // Muestra el contenido en la ventana
        sf::sleep(sf::milliseconds(1000 / 60));
    }

    return 0;
}
