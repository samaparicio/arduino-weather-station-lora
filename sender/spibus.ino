/* In SPI, setting CS = low is to turn on and setting CS high = turn off */


void talkToRadio(){
  digitalWrite(SD_CS, HIGH);
  //digitalWrite(RFM95_CS, LOW); //apparently the libraries bring it low anyways, so it's best not to do it prematurely
  delay(1);
}

void talkToSD(){
  digitalWrite(RFM95_CS, HIGH);
  //digitalWrite(SD_CS, LOW); //apparently the libraries bring it low anyways, so it's best not to do it prematurely
  delay(1);
}

