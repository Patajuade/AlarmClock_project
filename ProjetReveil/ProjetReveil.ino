#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> //librairie pour la carte PCA9685
#include <LCD.h> // pour l'écran LCD
#include <LiquidCrystal_I2C.h> // pour gérer l'écran LCD avec les pins SDA et SCL
#include <DS3231.h> // livrairie de l'horloge

DS3231  Clock(SDA, SCL);// on règle Clock pour utiliser le bus i2c

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); 

#define MIN_PULSE_WIDTH 650
#define MAX_PULSE_WIDTH 2350
#define DEFAULT_PULSE_WIDTH 1500
#define FREQUENCY 50


LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);//constructeur de LiquidCrystal => I2C_ADDR, EN_pin, RW_pin, RS_pin, D4_pin, D5_pin, D6_pin, D7_pin);
//const uint8_t servonum = 0; // 

const int Ok_Button = 53; // port numérique lié au bouton poussoir
const int Plus_Button = 52; // port numérique lié au bouton poussoir


int heureAlarm, minuteAlarm, secondeAlarm = 0; //On initialise les variables int heureAlarm, minuteAlarm et secondeAlarm, et on leur donne 0 comme valeur de base
bool alarmIsOn = false; //On initialise un booleen alarmIsOn, et on le set à false

void lcdSetup(){ //Methode comprenant tout le setup de base de l'écran, qu'on appellera dans le setup de base
  lcd.begin (20,4); //on règle l'écran 
  lcd.setBacklightPin(3,POSITIVE); // règle la lumière de fond de l'affichage  avec les arguments suivants => BL, BL_POL (BackLight, BackLight_Polarity)
  lcd.setBacklight(HIGH); //règle l'intensité de la lumière
  lcd.setCursor(0,1); //curseur à la première case de la deuxième ligne
}

void clockSetup(){//Methode comprenant tout le setup de base de l'horloge, qu'on appellera dans le setup de base
  Clock.begin(); //init de l'horloge
  Clock.setTime(8, 30, 0); // règle l'horloge(24hr format)
  Clock.setDate(1, 1, 2020); // Set the date to DD/MM/YYYY
}

void servoSetup(){ //Methode permettant d'initialiser les servos, qu'on appellera dans le setup de base
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
}

void buttonsSetup(){ //Methode permettant d'initialiser les boutons, qu'on appellera dans le setup de base
  pinMode(Ok_Button, INPUT); // réglage du port du bouton en mode ENTREE 
  pinMode(Plus_Button, INPUT); // réglage du port du bouton en mode ENTREE
}

int pulseWidth(int angle)
{
  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}

void hit(int bell, int wait) //methode permettant de dire à un moteur quel mouvement faire
{
  pwm.setPWM(bell, 1, pulseWidth(70));
  delay(100);
  pwm.setPWM(bell, 1, pulseWidth(0));
  if(wait<100)
    wait = 100;
  delay(wait-100);
}

void playSound(){ //Méthode permettant de jouer la mélodie
  hit(0, 400); // active le moteur associé à la pin pwm 0, attend 400ms
  hit(2, 400); //idem avec la pin pwm 2,...
  hit(4, 600);
  hit(5, 600);
  hit(3, 400);
  hit(4, 800);
  hit(0, 400);
  hit(2, 400);
  hit(4, 600);
  hit(3, 400);
  hit(2, 400);
  hit(0, 400);
  hit(1, 400);
}

bool isPlaying = false; //Met la valeur initiale de isPlaying à false

void alarm(){
  if (isPlaying)
    return;
  if (Clock.getTime().hour == heureAlarm && Clock.getTime().min == minuteAlarm && Clock.getTime().sec == secondeAlarm) 
  { 
    isPlaying = true;
    playSound();
  }
  else{
    isPlaying = false;
  }
  if(alarmIsOn) printAlarm(heureAlarm, minuteAlarm, secondeAlarm);
}

void ShowClockOnLCD(){ //méthode permettant d'afficher l'heure et la date sur l'écran LCD
  lcd.setCursor(0,0); //met le curseur à la case 0 sur la ligne 0
  lcd.print("Time : "); // affiche "Time : " sur l'écran lcd
  lcd.print(Clock.getTimeStr()); // Affiche l'heure via la méthode getTimeStr()
  lcd.setCursor(0,1); //met le curseur à la case 0 sur la ligne 1
  lcd.print("Date : "); //affiche "Date:" sur l'écran LCD
  lcd.print(Clock.getDateStr()); // affiche la date via la méthode getDateStr()
}

void showEntry(String entry, int value){
  lcd.setCursor(0,2);
  lcd.print(entry);
  lcd.print(" : ");
  lcd.print(value);
}

void resetLine(int line){
  lcd.setCursor(0,line);//on set le curseur à la ligne reçue en arguement
  lcd.print("                    ");//On écrase la valeur de la ligne
}

int calculNbJour(int mois){
  switch (mois){
   case 1://Janvier
    return 31;
   case 2://Février
    return 29;
   case 3://Mars
    return 31;
   case 4://Avril Lavigne
    return 30;
   case 5://Mai
    return 31;
   case 6://Juin
    return 30;
   case 7://Juillet
    return 31;
   case 8://Aout
    return 31;
   case 9://Septembre
    return 30;
   case 10://Octobre
    return 31;
   case 11://Novembre
    return 30;
   case 12://Décembre
    return 31;
  }
}
void printTime(int heure, int minute, int seconde){
  lcd.setCursor(0,0);
  lcd.print("Time : ");
  lcd.print(heure);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(seconde);
}
void printDate(int annee, int mois, int jour){
  lcd.setCursor(0,1);
  lcd.print("Date : ");
  lcd.print(annee);
  lcd.print(":");
  lcd.print(mois);
  lcd.print(":");
  lcd.print(jour);
}

void printAlarm(int heure, int minute, int seconde){
  lcd.setCursor(3,3);
  lcd.print("alarm : ");
  lcd.print(heure);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(seconde);
}

void printTimeDate(int annee, int mois, int jour, int heure, int minute, int seconde){
  lcd.clear();
  printDate(annee, mois, jour);
  printTime(heure, minute, seconde);
}

int setEntry(String entry, int value, int valueMax, int valueMin){
  bool isValueSetUp = false;
  showEntry(entry, value);
  while(!isValueSetUp){
    if(safeButtonRead(Plus_Button)){
      if(value < valueMax) value++;
      else if (value == valueMax) value = valueMin;
      resetLine(2);
      showEntry(entry, value);
    }
    if(safeButtonRead(Ok_Button)){
      isValueSetUp = true;
    }
  }
  return value;
}

void alarmSettings(){
  printTime(heureAlarm, minuteAlarm, secondeAlarm);
  heureAlarm = setEntry("Heure", heureAlarm, 23, 0);
  printTime(heureAlarm, minuteAlarm, secondeAlarm);
  minuteAlarm = setEntry("Minutes", minuteAlarm,  59, 0);
  printTime(heureAlarm, minuteAlarm, secondeAlarm);
  secondeAlarm = setEntry("Secondes", secondeAlarm, 59, 0);
  alarmIsOn = true;
}

void clockSettings(){
  int annee, mois, jour, heure, minute, seconde;
  annee = setEntry("Annee", annee, 2040, 2010);
  printTimeDate(annee, mois, jour, heure, minute, seconde);
  mois = setEntry("Mois", mois, 12, 1);//TODO améliorer
  printTimeDate(annee, mois, jour, heure, minute, seconde);
  jour = setEntry("Jour", jour, calculNbJour(mois), 1);
  printTimeDate(annee, mois, jour, heure, minute, seconde);
  heure = setEntry("Heure", heure, 23, 0);
  printTimeDate(annee, mois, jour, heure, minute, seconde);
  minute = setEntry("Minutes", minute,  59, 0);
  printTimeDate(annee, mois, jour, heure, minute, seconde);
  seconde = setEntry("Secondes", seconde, 59, 0);
  Clock.setTime(heure, minute, seconde);     // Set the time to 12:00:00 (24hr format)
  Clock.setDate(jour, mois, annee);   // Set the date to DD/MM/YYYY
}

bool safeButtonRead(int button){
  if(digitalRead(button) == LOW){//HIGH pullDown LOW Pull Up => 5v=>res=>inter+dp
    do{
      delay(5);
    }while(digitalRead(button) == LOW);
    return true;
  }
  return false;
}

void activateSettingsMenu(){
//set avec les boutons test
  if (safeButtonRead(Ok_Button)) { // si le bouton est pressé ...
    lcd.clear();
    settingsSelector();
  }
}

void showMenu(String value){
  lcd.setCursor(0,0);
  lcd.print(value);  
}

int catchButtons(){
  int menuChoice = 0;
  bool isMenuChanging = true;
  bool isOkPressed = false;
  int menuLastChoice = 3;
  while(!isOkPressed){
    if(safeButtonRead(Plus_Button)){
      if(menuChoice < menuLastChoice) menuChoice++;
      else if(menuChoice == menuLastChoice) menuChoice = 0;
      isMenuChanging = true;
    }
    if(safeButtonRead(Ok_Button)){ //Si le bouton Ok est appuyé...
      isOkPressed = true; // set isOkPressed à true
    }
    if(isMenuChanging){
      lcd.clear();
      if(menuChoice==0) showMenu("Setup Alarm ?");
      if(menuChoice==1) showMenu("Setup Time ?");
      if(menuChoice==2 && alarmIsOn ) showMenu("Deactivate Alarm");
      if(menuChoice==2 && !alarmIsOn ) showMenu("Activate Alarm");
      if(menuChoice==3) showMenu("return..."); //si menuChoice vaut 2, on affiche showMenu
      isMenuChanging=false; //set isMenuChanging à false
    }
  }  
  return menuChoice; //retourne la valeur de menuChoice
}

void settingsSelector(){
  int menuChoice = catchButtons(); // récupère la valeur du menu à afficher si on presse un bouton
  if(menuChoice==1) clockSettings(); //si menuChoice vaut 1, on appelle clockSettings
  if(menuChoice==0) alarmSettings(); //si menuChoice vaut 0, on appelle alarmSettings
  if(menuChoice==2) alarmIsOn = !alarmIsOn; //on inverse l'état de l'alarme, si elle était éteinte, on l'allume et inversement
  lcd.clear();//enlève tout l'affichage de l'écran
}

void setup() 
{
  Wire.begin();//initialise la lib Wire
  lcdSetup();//initialise la lib lcd
  clockSetup();//initialise l'horloge
  servoSetup();//initialise les servo
  buttonsSetup();//initialise les boutons
}

void loop() {
  ShowClockOnLCD(); //Affiche l'horloge et la date
  activateSettingsMenu(); //check s'il faut activer les settings et les lance si c'est le cas
  alarm();
}
