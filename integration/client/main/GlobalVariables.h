// GlobalVariables.h
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include <Audio.h>

// 共通変数の外部宣言
extern SDClass theSD;
extern AudioClass *theAudio;

//extern File myFile;

#endif /* GLOBALVARIABLES_H */
