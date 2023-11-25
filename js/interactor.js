
class Interactor {

  constructor(types,n){

    this.n = n ;
    this.types = types ;
    this.particles = [] ;

    this.createParticles();
    this.A = [] ;
    this.radio = [] ;
    this.min = [] ;

    for (var x = 0 ; x < this.types ; x++) {
      var filaA = [] ;
      var filaR = [] ;
      var filaM = [] ;
      for(var y = 0 ; y < this.types ; y++){

        var s = 1 ;
        if (random(100) < 50 ){ s = -1; }
        filaA.push(random(0.3,1) * s);
        filaR.push(random(70,250));
        filaM.push(random(30,50));
      }
      this.A.push(filaA);
      this.radio.push(filaR);
      this.min.push(filaM);
    }






  }

  createParticles(){

    for(var i = 0 ; i < this.n ; i++ ){
       var type = floor(random(this.types));
       this.particles.push( new Particle(type) );
    }

  }

  aplayForces(k){
 
    for(var part = 0 ; part < this.n ; part++ ){

      this.particles[part].display_(this.types);
      this.particles[part].move();
      //this.particles[part].limit();

      var sum = createVector(0,0);

      for(var other = 0 ; other < this.n ; other++ ){

        if ( this.particles[part] != this.particles[other] ){

            var actual = this.particles[part].p.copy();
            var other_ = this.particles[other].p.copy();

            var actual_type = this.particles[part].type;
            var other_type = this.particles[other].type;

            var sub = p5.Vector.sub(other_,actual);

            if (sub.x > width/2) {
                sub.x -= width
            }

            if (sub.x < -width/2) {
                sub.x += width
            }

            if (sub.y > height/2) {
                sub.y -= height
            }

            if (sub.y < -height/2) {
                sub.y += height
            }

            var dist = sub.mag();
            var uni = p5.Vector.normalize(sub) ;

            //var k = 0.5 ;

            if ( dist < this.min[actual_type][other_type] ){

                var minForce = abs(this.A[actual_type][other_type])*-2.5 ;
                var repulse = map(dist,0,this.min[actual_type][other_type],1,0);// minForce * dist / this.min[actual_type][other_type] ;
                var repuldeForce = uni.copy().mult(repulse * k * minForce) ;
                sum.add(repuldeForce) ;

            }

            if ( dist > this.min[actual_type][other_type] && dist < this.radio[actual_type][other_type]){

              var maxForce = this.A[actual_type][other_type] ;
              var ampl = map(dist,0,this.radio[actual_type][other_type],1,0) ;
              var ampl = uni.copy().mult(ampl* k * maxForce) ;
              sum.add(ampl) ;

            }



        }

      }

      this.particles[part].aplayForce(sum);
    }

  }

}
