import 'dart:ffi';

class ChaoStruct extends Struct {
  //factory ChaoStruct.allocate(int )
  @Uint8()
  external int GBAChao;
  @Uint8()
  external int GBAEgg;
  @Array(8)
  external Array<Uint8> GBABerry;
  @Uint16()
  external int padding;

  @Uint8()
  external int GBARing;
  @Uint8()
  external int BootMethod;
  @Uint8()
  external int Birthplace;
  @Array(7)
  external Array<Char> name;

  @Uint8()
  external int GBAType;
  @Uint8()
  external int GBASkin;
  @Uint8()
  external int GBAMood;
  @Uint8()
  external int GBABelly;
  @Uint8()
  external int GBASleepy;
  @Uint8()
  external int GBALonelyness;
  @Uint8()
  external int padding2;

  @Array(8)
  external Array<Uint8> Exp;
  @Array(8)
  external Array<Uint8> Abl;
  @Array(8)
  external Array<Uint8> Lev;
  @Array(8)
  external Array<Uint16> Skill;
  @Array(16)
  external Array<Uint16> GBAPalette;
  @Array(16)
  external Array<Uint8> rmsg;

  @Uint32()
  external int runaway;
  @Array(4)
  external Array<Uint8> dummy;
  @Uint8()
  external int type;
  @Uint8()
  external int place;
  @Int16()
  external int like;
  @Char()
  external int ClassNum;

  @Uint16()
  external int age;
  @Uint16()
  external int old;
  @Uint16()
  external int life;
  @Uint16()
  external int LifeMax;
  @Uint16()
  external int nbSucceed;
  @Array(5)
  external Array<Uint32> ChaoID; // This is a small struct called CHAO_ID
  @Uint32()
  external int LifeTimer;
  @Array(0x38)
  external Array<Uint8> body; // Struct AL_BODY_INFO
  @Array(0x2A)
  external Array<Uint8> race; // Struct AL_RACE_PERSONAL_INFO
  @Array(0x0C)
  external Array<Uint8> karate; // Struct AL_KARATE_PERSONAL_INFO
  @Array(0x0C)
  external Array<Uint8> PartsBTL; // Struct AL_PARTS
  @Array(0x3C)
  external Array<Uint8> emotion; // Struct AL_EMOTION
  @Array(0x2D8)
  external Array<Uint8> knowledgeBTL; // Struct AL_KNOWLEDGE_BTL
  @Array(0xA4)
  external Array<Uint8> gene; // Struct AL_GENE
  @Uint32()
  external int IsInitializedDX;
  @Array(0x0C)
  external Array<Uint8> partsDX; // Struct AL_PARTS
  @Array(0x0F)
  external Array<Uint8> raceDX; // Struct AL_RACE_PERSONAL_INFO_DX
  @Array(0x24)
  external Array<Uint8> knowledgeDX; // Struct AL_KNOWLEDGE_DX
}
