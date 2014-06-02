#ifndef dSINGLE
#define dSINGLE
#endif

bool reset=false;
#include <ode/odeconfig.h>
#include <assert.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "cSerwer.h"



#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

int licznik=0;
int wynikGora=0;
int wynikDol=0;
bool bramkaGora=false;
bool bramkaDol=false;


static dWorldID world;
static dSpaceID space;

static dBodyID zbijak1body;
static dGeomID zbijak1geom;

static dBodyID zbijak2body;
static dGeomID zbijak2geom;

static dBodyID krazekbody;
static dGeomID krazekgeom;

static bool ruch=false;

static dBodyID bandabody[12]; //7 i 6 bramki
static dGeomID bandageom[12];

sRamkaV Predkosci[2];
cSerwer serwer;


static dJointGroupID contactgroup;

static bool show_contacts = false;

#define CYLRADIUS    0.8
#define CYLLENGTH    0.25


#define BANDASZER 1.0
#define BANDAWYS 1.0
#define BANDADLUG 18.0

#define KRAZEKWYS 0.4
#define KRAZEKPROMIEN 0.5

#define DZIELNIK1 5.0 
#define DZIELNIK2 3.0
#define DZIELNIK3 6.0

#define XPOCZ 6.0
#define YPOCZ 10.0

#define LIMIT 3


// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  assert(o1);
  assert(o2);

  if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
  {
      fprintf(stderr,"testing space %p %p\n", (void*)o1, (void*)o2);
    // colliding a space with something
    dSpaceCollide2(o1,o2,data,&nearCallback);
    // Note we do not want to test intersections within a space,
    // only between spaces.
    return;
  }

  
  if((o1==krazekgeom&&o2==bandageom[7])||(o2==krazekgeom&&o1==bandageom[7]))
	bramkaGora=true;

  if((o1==krazekgeom&&o2==bandageom[6])||(o2==krazekgeom&&o1==bandageom[6]))
	bramkaDol=true;
	  
  


  const int N = 30;
  dContact contact[N];
  int n = dCollide (o1,o2,N,&(contact[0].geom),sizeof(dContact));

  if (n > 0) 
  {
    for (int i=0; i<n; i++) 
    {
      contact[i].surface.mode = dContactApprox1 | dContactBounce ;
      contact[i].surface.mu = 0.01; // was: dInfinity
	  contact[i].surface.bounce = 0.9;
	  contact[i].surface.bounce_vel = 0.0001;
      dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
      dJointAttach (c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
    }
  }
}


// start simulation - set viewpoint

static void start()
{
  dAllocateODEDataForThread(dAllocateMaskAll);

  static float xyz[3] = {5.9466972,14.602594,16.059916};
  static float hpr[3] = {-90.0000f,-78.0000f,0.0000f};
  dsSetViewpoint (xyz,hpr);
  printf("Zerowanie wynikow kalwiszem 'R': \n" );
  printf("Wyniki: \n" );

}


// called when a key pressed

static void command (int cmd)
{
  switch (cmd) 
  {
    case ' ':
      break;

	case 'r': case 'R':
		{
			wynikGora=0;
			wynikDol=0;
			//system("cls");
			//printf("Zerowanie wynikow kalwiszem 'R': \n" );
			printf("Wyniki: \n" );
			dReal* pozycja=new dReal[3];
			const dReal* bandapoz=dBodyGetPosition(bandabody[7]);
			const dReal* bandapoz2=dBodyGetPosition(bandabody[6]);
			pozycja[0]=bandapoz[0];
			pozycja[1]=bandapoz[1]+6;
			pozycja[2]=0;
			dBodySetPosition(krazekbody,pozycja[0],pozycja[1],pozycja[2]);
			pozycja[1]=bandapoz[1]+2;
			dBodySetPosition(zbijak1body,pozycja[0],pozycja[1],pozycja[2]);
			pozycja[1]=bandapoz2[1]-2;
			dBodySetPosition(zbijak2body,pozycja[0],pozycja[1],pozycja[2]);
			dBodySetLinearVel(krazekbody,0,0,0);
			break;
		}
  }
}



// simulation loop

static void simLoop (int pause)
{
  /*if(!ruch)
  dBodySetLinearVel(zbijak1body, 0, 0, 0);
  ruch=false;*/
	Sleep(5);
	serwer.PobierzV(Predkosci);
	dBodySetLinearVel(zbijak1body, Predkosci[0].xV, Predkosci[0].yV, 0);
	dBodySetLinearVel(zbijak2body, Predkosci[1].xV, Predkosci[1].yV, 0);
  dReal R[12];
	
	for(int i=0; i<12; i++)
		R[i]=0;
	
	R[0]=1;
	R[5]=1;
	R[10]=1;
  
  const dReal* krazekpoz=dBodyGetPosition(krazekbody);
  dBodySetRotation(krazekbody,R);
  dBodySetPosition(krazekbody,krazekpoz[0],krazekpoz[1],0);

  const dReal* zbijak1poz=dBodyGetPosition(zbijak1body);
  dReal pozycjaY;
  dBodySetRotation(zbijak1body,R);
  if(zbijak1poz[1]>(YPOCZ-CYLRADIUS))
	  pozycjaY=YPOCZ-CYLRADIUS;
  else
	  pozycjaY=zbijak1poz[1];
  dBodySetPosition(zbijak1body,zbijak1poz[0],pozycjaY,0);

  const dReal* zbijak2poz=dBodyGetPosition(zbijak2body);
  if(zbijak2poz[1]<(YPOCZ+CYLRADIUS))
	  pozycjaY=YPOCZ+CYLRADIUS;
  else
	  pozycjaY=zbijak2poz[1];
  dBodySetRotation(zbijak2body,R);
  dBodySetPosition(zbijak2body,zbijak2poz[0],pozycjaY,0);

  if(dBodyGetLinearVel(krazekbody)[0]>LIMIT)
	  dBodySetLinearVel(krazekbody,10,dBodyGetLinearVel(krazekbody)[1],0);

  if(dBodyGetLinearVel(krazekbody)[0]<-LIMIT)
	  dBodySetLinearVel(krazekbody,-LIMIT,dBodyGetLinearVel(krazekbody)[1],0);

  if(dBodyGetLinearVel(krazekbody)[1]<-LIMIT)
	  dBodySetLinearVel(krazekbody,dBodyGetLinearVel(krazekbody)[0],-LIMIT,0);

  if(dBodyGetLinearVel(krazekbody)[1]>LIMIT)
	  dBodySetLinearVel(krazekbody,dBodyGetLinearVel(krazekbody)[0],LIMIT,0);

  //obs³uga bramek:
  if(bramkaGora)
  {
	wynikDol++;
	dReal* pozycja=new dReal[3];
	const dReal* bandapoz=dBodyGetPosition(bandabody[7]);
	const dReal* bandapoz2=dBodyGetPosition(bandabody[6]);
	pozycja[0]=bandapoz[0];
	pozycja[1]=bandapoz[1]+6;
	pozycja[2]=0;
	dBodySetPosition(krazekbody,pozycja[0],pozycja[1],pozycja[2]);
	pozycja[1]=bandapoz[1]+2;
	dBodySetPosition(zbijak1body,pozycja[0],pozycja[1],pozycja[2]);
	pozycja[1]=bandapoz2[1]-2;
	dBodySetPosition(zbijak2body,pozycja[0],pozycja[1],pozycja[2]);
	dBodySetLinearVel(krazekbody,0,0,0);
	Sleep(1000);
	bramkaGora=false;
	printf("Dolny %i : %i Gorny \n",wynikDol,wynikGora );
  }

    if(bramkaDol)
  {
	wynikGora++;
	dReal* pozycja=new dReal[3];
	const dReal* bandapoz=dBodyGetPosition(bandabody[6]);
	const dReal* bandapoz2=dBodyGetPosition(bandabody[7]);
	pozycja[0]=bandapoz[0];
	pozycja[1]=bandapoz[1]-6;
	pozycja[2]=0;
	dBodySetPosition(krazekbody,pozycja[0],pozycja[1],pozycja[2]);
	pozycja[1]=bandapoz[1]+-2;
	dBodySetPosition(zbijak2body,pozycja[0],pozycja[1],pozycja[2]);
	pozycja[1]=bandapoz2[1]+2;
	dBodySetPosition(zbijak1body,pozycja[0],pozycja[1],pozycja[2]);
	dBodySetLinearVel(krazekbody,0,0,0);
	Sleep(1000);
	bramkaDol=false;
	printf("Dolny %i : %i Gorny \n",wynikDol,wynikGora );
  }


	  

  dSpaceCollide (space,0,&nearCallback);  
  if (!pause)
  {
    dWorldQuickStep (world, 0.05); 
  }
  dJointGroupEmpty (contactgroup);

  dsSetColorAlpha (1,0,0,1);
  

  const dReal *CPos = dBodyGetPosition(zbijak1body);
  const dReal *CRot = dBodyGetRotation(zbijak1body);
  float cpos[3] = {CPos[0], CPos[1], CPos[2]};
  float crot[12] = { CRot[0], CRot[1], CRot[2], CRot[3], CRot[4], CRot[5], CRot[6], CRot[7], CRot[8], CRot[9], CRot[10], CRot[11] };
  dsDrawCylinder
  (
    cpos,
    crot,
    CYLLENGTH,
    CYLRADIUS
  );

  dsSetColorAlpha (0,1,0,1);

    const dReal *C2Pos = dBodyGetPosition(zbijak2body);
  const dReal *C2Rot = dBodyGetRotation(zbijak2body);
  float c2pos[3] = {C2Pos[0], C2Pos[1], C2Pos[2]};
  float c2rot[12] = { C2Rot[0], C2Rot[1], C2Rot[2], C2Rot[3], C2Rot[4], C2Rot[5], C2Rot[6], C2Rot[7], C2Rot[8], C2Rot[9], C2Rot[10], C2Rot[11] };
  dsDrawCylinder
  (
    c2pos,
    c2rot,
    CYLLENGTH,
    CYLRADIUS
  ); 

    dsSetColorAlpha (1,0,0,1);

  const dReal *KPos = dBodyGetPosition(krazekbody);
  const dReal *KRot = dBodyGetRotation(krazekbody);
  float kpos[3] = {KPos[0], KPos[1], KPos[2]};
  float krot[12] = { KRot[0], KRot[1], KRot[2], KRot[3], KRot[4], KRot[5], KRot[6], KRot[7], KRot[8], KRot[9], KRot[10], KRot[11] };
  dsDrawCylinder
  (
    kpos,
    krot,
    KRAZEKWYS,
    KRAZEKPROMIEN
  ); 



  /*
  float bwym[3] = {BANDASZER, BANDADLUG, BANDAWYS};
  const dReal *BPos = dBodyGetPosition(testbody);
  const dReal *BRot = dBodyGetRotation(testbody);
  float bpos[3] = {BPos[0], BPos[1], BPos[2]};
  float brot[12] = { BRot[0], BRot[1], BRot[2], BRot[3], BRot[4], BRot[5], BRot[6], BRot[7], BRot[8], BRot[9], BRot[10], BRot[11] };
  dsDrawBox
  (
    bpos,
    brot,
    bwym
  ); // single precision

  */





  float bwym[3] = {BANDASZER, BANDADLUG, BANDAWYS};
  float bwym2[3] = {BANDASZER, BANDADLUG/DZIELNIK1, BANDAWYS};
  float bwym3[3] = {BANDASZER, BANDADLUG/DZIELNIK2, BANDAWYS};
  float bwym4[3] = {BANDASZER, BANDADLUG/DZIELNIK3, BANDAWYS};

  for(int i=0; i<12; i++)
  {
  const dReal *BPos = dBodyGetPosition(bandabody[i]);
  const dReal *BRot = dBodyGetRotation(bandabody[i]);
  float bpos[3] = {BPos[0], BPos[1], BPos[2]};
  float brot[12] = { BRot[0], BRot[1], BRot[2], BRot[3], BRot[4], BRot[5], BRot[6], BRot[7], BRot[8], BRot[9], BRot[10], BRot[11] };

  if (i>7)
	 dsDrawBox ( bpos, brot, bwym4 );
  else if (i>5)
	 dsDrawBox ( bpos, brot, bwym3 );
  else if (i>1)
	  dsDrawBox ( bpos, brot, bwym2 );
  else
	  dsDrawBox ( bpos, brot, bwym );
  
  }
  
}


int main (int argc, char **argv)
{
  dMass m;

  	dReal R[12];
	dReal R2[12];
	dReal R3[12];
	
	for(int i=0; i<12; i++)
		R[i]=0;
	
	R[0]=1;
	R[5]=1;
	R[10]=1;

	
  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = &command;
  fn.stop = 0;
  fn.path_to_textures = "D:/Dokumenty/Visual Studio 2010/ode-0.13/drawstuff/textures";
  fn.path_to_textures = "F:/Visual/ode-0.13/drawstuff/textures";

  //rejestracja klientow

  
  serwer.Rejestracja(0);
  serwer.Rejestracja(1);

  
  // create world
  dInitODE2(0);
  world = dWorldCreate();
  space = dHashSpaceCreate (0);
  contactgroup = dJointGroupCreate (0);
  dWorldSetGravity (world,0,0,-5);
  dWorldSetQuickStepNumIterations (world, 5);
  dWorldSetAutoDisableAngularThreshold(world,10000);
  //dWorldSetCFM (world,100);
  
  dCreatePlane (space,0,0,1, 0.0);

  zbijak1body = dBodyCreate (world);
    dBodySetRotation(zbijak1body,R);
 // dBodySetKinematic(zbijak1body);
  dMassSetCylinder (&m,100,3,CYLRADIUS,CYLLENGTH);
  dBodySetMass (zbijak1body,&m);
  zbijak1geom = dCreateCylinder(0, CYLRADIUS, CYLLENGTH);
  dGeomSetBody (zbijak1geom,zbijak1body);
  dBodySetPosition (zbijak1body, 5.5, 10, CYLLENGTH/2);
  dSpaceAdd (space, zbijak1geom);

    zbijak2body = dBodyCreate (world);
    dBodySetRotation(zbijak2body,R);
 // dBodySetKinematic(zbijak2body);
  dBodySetMass (zbijak2body,&m);
  zbijak2geom = dCreateCylinder(0, CYLRADIUS, CYLLENGTH);
  dGeomSetBody (zbijak2geom,zbijak2body);
  dBodySetPosition (zbijak2body, 5.5, 15, CYLLENGTH/2);
  dSpaceAdd (space, zbijak2geom);


  krazekbody = dBodyCreate (world);
  dBodySetRotation (krazekbody,R);
  dMassSetCylinder (&m,0.1,3,KRAZEKPROMIEN,KRAZEKWYS);
  dBodySetMass (krazekbody,&m);
  krazekgeom = dCreateCylinder(0, KRAZEKPROMIEN, KRAZEKWYS);
  dGeomSetBody (krazekgeom,krazekbody);
  dBodySetPosition (krazekbody, 5.5, 12, KRAZEKWYS/2);
  dSpaceAdd (space, krazekgeom);
  



 /* 
	 testbody = dBodyCreate(world);

	  dBodySetRotation(testbody,R);
	  dBodySetKinematic(testbody);
	  testgeom = dCreateBox(0, BANDASZER, BANDADLUG, BANDAWYS);
	  dBodySetPosition (testbody, 0, 0, BANDAWYS/2);
	  dGeomSetBody(testgeom, testbody);
	  dSpaceAdd(space,testgeom);


	  */
	  

  
  for(int i=0;i<12;i++)
  {
	  bandabody[i]= dBodyCreate(world);
	  //dMassSetBoxTotal(&mbanda,100000,BANDASZER, BANDADLUG, BANDAWYS);
	  //dBodySetMass(bandabody[i],&mbanda);
	  
	  dBodySetRotation(bandabody[i],R);
	  dBodySetKinematic(bandabody[i]);
	  
  }

  for (int i=0; i<2; i++)
  {
	  bandageom[i] = dCreateBox(0, BANDASZER, BANDADLUG, BANDAWYS);
  }

  for (int i=2; i<6; i++)
  {
	  bandageom[i] = dCreateBox(0, BANDASZER, BANDADLUG/DZIELNIK1, BANDAWYS);
  }

  for (int i=6; i<8; i++)
  {
	  bandageom[i] = dCreateBox(0, BANDASZER, BANDADLUG/DZIELNIK2, BANDAWYS);
  }

  for (int i=8; i<12; i++)
  {
	  bandageom[i] = dCreateBox(0, BANDASZER, BANDADLUG/DZIELNIK3, BANDAWYS);
  }



  	for(int i=0; i<12; i++)
		{dSpaceAdd(space,bandageom[i]);
		dGeomSetBody(bandageom[i],bandabody[i]);}

		
  	for(int i=0; i<12; i++)
		{ R[i]=0; R2[i]=0; R3[i]=0;}
	
	R[1]=1;
	R[4]=-1;
	R[10]=1;
	R2[0]=0.70710678118;
	R2[1]=0.70710678118;
	R2[4]=-0.70710678118;
	R2[5]=0.70710678118;
	R2[10]=1;
	R3[0]=-0.70710678118;
	R3[1]=0.70710678118;
	R3[4]=-0.70710678118;
	R3[5]=-0.70710678118;
	R3[10]=1;


  for (int i=2; i<8; i++)
  dBodySetRotation(bandabody[i],R);

  for (int i=8; i<10; i++)
  dBodySetRotation(bandabody[i],R2);

  for (int i=10; i<12; i++)
  dBodySetRotation(bandabody[i],R3);

  dBodySetPosition(bandabody[0],XPOCZ-BANDADLUG/DZIELNIK1-BANDADLUG/DZIELNIK2/2-BANDASZER/2,YPOCZ,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[1],XPOCZ+BANDADLUG/DZIELNIK1+BANDADLUG/DZIELNIK2/2+BANDASZER/2,YPOCZ,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[2],XPOCZ-BANDADLUG/DZIELNIK1/2-BANDADLUG/DZIELNIK2/2,YPOCZ+BANDADLUG/2-BANDASZER/2,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[3],XPOCZ+BANDADLUG/DZIELNIK1/2+BANDADLUG/DZIELNIK2/2,YPOCZ+BANDADLUG/2-BANDASZER/2,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[4],XPOCZ-BANDADLUG/DZIELNIK1/2-BANDADLUG/DZIELNIK2/2,YPOCZ-BANDADLUG/2+BANDASZER/2,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[5],XPOCZ+BANDADLUG/DZIELNIK1/2+BANDADLUG/DZIELNIK2/2,YPOCZ-BANDADLUG/2+BANDASZER/2,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[6],XPOCZ,YPOCZ+BANDADLUG/2-BANDASZER/2+0.5,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[7],XPOCZ,YPOCZ-BANDADLUG/2+BANDASZER/2-0.5,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[8],XPOCZ-BANDADLUG/DZIELNIK2/2-BANDADLUG/DZIELNIK1+0.2*BANDASZER,YPOCZ+BANDADLUG/2-BANDASZER-0.2*BANDASZER,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[9],XPOCZ+BANDADLUG/DZIELNIK2/2+BANDADLUG/DZIELNIK1-0.2*BANDASZER,YPOCZ-BANDADLUG/2+BANDASZER+0.2*BANDASZER,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[10],XPOCZ+BANDADLUG/DZIELNIK2/2+BANDADLUG/DZIELNIK1-0.2*BANDASZER,YPOCZ+BANDADLUG/2-BANDASZER-0.2*BANDASZER,BANDAWYS/2-0.5);
  dBodySetPosition(bandabody[11],XPOCZ-BANDADLUG/DZIELNIK2/2-BANDADLUG/DZIELNIK1+0.2*BANDASZER,YPOCZ-BANDADLUG/2+BANDASZER+0.2*BANDASZER,BANDAWYS/2-0.5);
  
  //ograniczenia predkosci
  //dBodySetLinearDampingThreshold(krazekbody, 1);

  // run simulation
  system("cls");
  dsSimulationLoop (argc,argv,800,600,&fn);
  
  dJointGroupEmpty (contactgroup);
  dJointGroupDestroy (contactgroup);
  
  for (int i=0; i<12; i++)
	dGeomDestroy(bandageom[i]);
	
  dGeomDestroy (krazekgeom);
  //dGeomDestroy (testgeom);
  dGeomDestroy (zbijak1geom);

  dSpaceDestroy (space);
  dWorldDestroy (world);
  
  dCloseODE();
  return 0;
}