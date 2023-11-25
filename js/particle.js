
function type2Color(i,types){
   return color( (360/types)*i,100,100);
}

class Particle {

  constructor(tipo) {

   this.p = createVector(random(width),random(height)) ;
   this.v = createVector(random(0,0),random(0,0) ) ;
   this.a = createVector(0,0) ;

   this.type = tipo ;
   this.radio = 5 ;
   this.drag = 0.5;

  }

  move(){
    this.v.mult(this.drag)
    this.v.add(this.a) ;
    this.p.add(this.v) ;
    this.p.x = (this.p.x + width)%width ;
    this.p.y = (this.p.y + height)%height ;
    this.a.mult(0);
  }

  aplayForce(f){
    this.a.add(f) ;
  }

  limit(){
   if(this.p.x > width || this.p.x < 0){
     this.v.x = this.v.x * -1 ;
   }

   if(this.p.y > height || this.p.y < 0){
     this.v.y = this.v.y * -1 ;
   }

  }

  display_( types ){
    noStroke();
    fill( type2Color(this.type,types) );
    ellipse(this.p.x,this.p.y,this.radio,this.radio) ;
  }

}
