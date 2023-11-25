var particles = []
var n = 300 ;
var types = 6 ;

var inte ;

function setup() {

  createCanvas(1080, 720);
  colorMode(HSB, 360, 100, 100);
  rSlider = createSlider(0, 100, 50);
  rSlider.position(20, 20);

  inte = new Interactor(types,n);

}

function draw() {

  background(0);

  inte.aplayForces(rSlider.value()/100);

}
