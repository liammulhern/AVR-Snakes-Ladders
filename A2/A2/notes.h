/*
 * notes.h
 *
 * Created: 23/10/2022 5:20:44 PM
 *  Author: LiamM
 */ 


#ifndef NOTES_H_
#define NOTES_H_

#define HALF 400
#define QUARTER 200
#define EIGHTH 100
#define DOTQUARTER 300

#define NOTE_B0(duration) {31, 50.0, 0, duration}
#define NOTE_C1(duration) {33, 50.0, 0, duration}
#define NOTE_CS1(duration) {35, 50.0, 0, duration}
#define NOTE_D1(duration) {37, 50.0, 0, duration}
#define NOTE_DS1(duration) {39, 50.0, 0, duration}
#define NOTE_E1(duration) {41, 50.0, 0, duration}
#define NOTE_F1(duration) {44, 50.0, 0, duration}
#define NOTE_FS1(duration) {46, 50.0, 0, duration}
#define NOTE_G1(duration) {49, 50.0, 0, duration}
#define NOTE_GS1(duration) {52, 50.0, 0, duration}
#define NOTE_A1(duration) {55, 50.0, 0, duration}
#define NOTE_AS1(duration) {58, 50.0, 0, duration}
#define NOTE_B1(duration) {62, 50.0, 0, duration}
#define NOTE_C2(duration) {65, 50.0, 0, duration}
#define NOTE_CS2(duration) {69, 50.0, 0, duration}
#define NOTE_D2(duration) {73, 50.0, 0, duration}
#define NOTE_DS2(duration) {78, 50.0, 0, duration}
#define NOTE_E2(duration) {82, 50.0, 0, duration}
#define NOTE_F2(duration) {87, 50.0, 0, duration}
#define NOTE_FS2(duration) {93, 50.0, 0, duration}
#define NOTE_G2(duration) {98, 50.0, 0, duration}
#define NOTE_GS2(duration) {104, 50.0, 0, duration}
#define NOTE_A2(duration) {110, 50.0, 0, duration}
#define NOTE_AS2(duration) {117, 50.0, 0, duration}
#define NOTE_B2(duration) {123, 50.0, 0, duration}
#define NOTE_C3(duration) {131, 50.0, 0, duration}
#define NOTE_CS3(duration) {139, 50.0, 0, duration}
#define NOTE_D3(duration) {147, 50.0, 0, duration}
#define NOTE_DS3(duration) {156, 50.0, 0, duration}
#define NOTE_E3(duration) {165, 50.0, 0, duration}
#define NOTE_F3(duration) {175, 50.0, 0, duration}
#define NOTE_FS3(duration) {185, 50.0, 0, duration}
#define NOTE_G3(duration) {196, 50.0, 0, duration}
#define NOTE_GS3(duration) {208, 50.0, 0, duration}
#define NOTE_A3(duration) {220, 50.0, 0, duration}
#define NOTE_AS3(duration) {233, 50.0, 0, duration}
#define NOTE_B3(duration) {247, 50.0, 0, duration}
#define NOTE_C4(duration) {262, 50.0, 0, duration}
#define NOTE_CS4(duration) {277, 50.0, 0, duration}
#define NOTE_D4(duration) {294, 50.0, 0, duration}
#define NOTE_DS4(duration) {311, 50.0, 0, duration}
#define NOTE_E4(duration) {330, 50.0, 0, duration}
#define NOTE_F4(duration) {349, 50.0, 0, duration}
#define NOTE_FS4(duration) {370, 50.0, 0, duration}
#define NOTE_G4(duration) {392, 50.0, 0, duration}
#define NOTE_GS4(duration) {415, 50.0, 0, duration}
#define NOTE_A4(duration) {440, 50.0, 0, duration}
#define NOTE_AS4(duration) {466, 50.0, 0, duration}
#define NOTE_B4(duration) {494, 50.0, 0, duration}
#define NOTE_C5(duration) {523, 50.0, 0, duration}
#define NOTE_CS5(duration) {554, 50.0, 0, duration}
#define NOTE_D5(duration) {587, 50.0, 0, duration}
#define NOTE_DS5(duration) {622, 50.0, 0, duration}
#define NOTE_E5(duration) {659, 50.0, 0, duration}
#define NOTE_F5(duration) {698, 50.0, 0, duration}
#define NOTE_FS5(duration) {740, 50.0, 0, duration}
#define NOTE_G5(duration) {784, 50.0, 0, duration}
#define NOTE_GS5(duration) {831, 50.0, 0, duration}
#define NOTE_A5(duration) {880, 50.0, 0, duration}
#define NOTE_AS5(duration) {932, 50.0, 0, duration}
#define NOTE_B5(duration) {988, 50.0, 0, duration}
#define NOTE_C6(duration) {1047, 50.0, 0, duration}
#define NOTE_CS6(duration) {1109, 50.0, 0, duration}
#define NOTE_D6(duration) {1175, 50.0, 0, duration}
#define NOTE_DS6(duration) {1245, 50.0, 0, duration}
#define NOTE_E6(duration) {1319, 50.0, 0, duration}
#define NOTE_F6(duration) {1397, 50.0, 0, duration}
#define NOTE_FS6(duration) {1480, 50.0, 0, duration}
#define NOTE_G6(duration) {1568, 50.0, 0, duration}
#define NOTE_GS6(duration) {1661, 50.0, 0, duration}
#define NOTE_A6(duration) {1760, 50.0, 0, duration}
#define NOTE_AS6(duration) {1865, 50.0, 0, duration}
#define NOTE_B6(duration) {1976, 50.0, 0, duration}
#define NOTE_C7(duration) {2093, 50.0, 0, duration}
#define NOTE_CS7(duration) {2217, 50.0, 0, duration}
#define NOTE_D7(duration) {2349, 50.0, 0, duration}
#define NOTE_DS7(duration) {2489, 50.0, 0, duration}
#define NOTE_E7(duration) {2637, 50.0, 0, duration}
#define NOTE_F7(duration) {2794, 50.0, 0, duration}
#define NOTE_FS7(duration) {2960, 50.0, 0, duration}
#define NOTE_G7(duration) {3136, 50.0, 0, duration}
#define NOTE_GS7(duration) {3322, 50.0, 0, duration}
#define NOTE_A7(duration) {3520, 50.0, 0, duration}
#define NOTE_AS7(duration) {3729, 50.0, 0, duration}
#define NOTE_B7(duration) {3951, 50.0, 0, duration}
#define NOTE_C8(duration) {4186, 50.0, 0, duration}
#define NOTE_CS8(duration) {4435, 50.0, 0, duration}
#define NOTE_D8(duration) {4699, 50.0, 0, duration}
#define NOTE_DS8(duration) {4978, 50.0, 0, duration}
#define REST(duration) {0, 0.0, 0, duration}


#endif /* NOTES_H_ */