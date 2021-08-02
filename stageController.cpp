#include "stageController.h"


//#define DEBUGLOGS_STAGE
#define HOSTPORT        9000
const QString HOSTIP = "192.168.1.101";
stageController::stageController(structScan * scanInfoPtr_arg)
{
    updateTimer = new QTimer(this);
    //connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePos()));
    //QTcpSocket tcpSocket;
    //if (tcpSocket.isOpen()==false || tcpSocket.isValid() ==false || tcpSocket.isWritable() == false)
        //tcpSocket.connectToHost(HOSTIP, HOSTPORT);
    unsigned int returnVal;
    if (readPlcMem32(302,&returnVal) != false) // so PLC is working so lets start the update timer
    {
        //updateTimer->start(500);
    }

    scanStartPosX = 0.0;
    scanStartPosZ = 0.0;
    scanInfoPtr = scanInfoPtr_arg;

    //clearServoStop();
    //resetMovetoX();
    //resetMovetoZ();
    //clearErrorReset();
    //clearServoStop();
    //clearScanAreaMarker();

    writePlcMem32(226,10000); //X-axis Jog
    writePlcMem32(228,10000); //Z-axis Jog
}

stageController::~stageController()
{
    delete updateTimer;
}

bool stageController::writePlcMem(short address, unsigned short value)
{
    QByteArray cmd;
    QByteArray rsp("No Response");
    QString addStrStart = "00"+QString::number(address);
    QString addStrEnd = "00"+QString::number(address);
    unsigned short valueBigEndian = qToBigEndian(value);
    QString hexValue = (QString("%1").arg(valueBigEndian,4,16,QLatin1Char('0'))).toUpper();
    QString cmdStr = "%01#WDD"+addStrStart+addStrEnd+hexValue+"**\r";
    cmd = cmdStr.toLocal8Bit();
    bool writeSuccess = false;
    QTcpSocket tcpSocket;


    //if (tcpSocket.isOpen()==false || tcpSocket.isValid() ==false)
        tcpSocket.connectToHost(HOSTIP, HOSTPORT);
    tcpSocket.write(cmd);

    tcpSocket.waitForReadyRead(500);

    rsp = tcpSocket.readAll();
    if (rsp.size()>0)
    {
        if (rsp.at(3)=='$')
            writeSuccess = true;
    }

    #ifdef DEBUGLOGS_STAGE
        qDebug()<<"writePlcMem cmdStr: "<<cmdStr<<"  rsp:"<<rsp<<"writeSuccess"<<writeSuccess;
    #endif

    return writeSuccess;
}

bool stageController::writePlcMem32(short address, unsigned int intValue)
{
    QByteArray cmd;
    QByteArray rsp("No Response");
    QString addStrStart = "00"+QString::number(address);
    QString addStrEnd = "00"+QString::number(address+1);
    unsigned short valueL = (intValue)&0xffff;
    unsigned short valueH = (intValue>>16)&0xffff;
    unsigned short valueLBigEndian = qToBigEndian(valueL);
    unsigned short valueHBigEndian = qToBigEndian(valueH);

    QString hexValueL = (QString("%1").arg(valueLBigEndian,4,16,QLatin1Char('0'))).toUpper();
    QString hexValueH = (QString("%1").arg(valueHBigEndian,4,16,QLatin1Char('0'))).toUpper();;


    QString cmdStr = "%01#WDD"+addStrStart+addStrEnd+hexValueL+hexValueH+"**\r";

    cmd = cmdStr.toLocal8Bit();
    bool writeSuccess = false;
    QTcpSocket tcpSocket;

    //if (tcpSocket.isOpen()==false || tcpSocket.isValid() ==false || tcpSocket.isWritable() == false)
        tcpSocket.connectToHost(HOSTIP, HOSTPORT);
    tcpSocket.write(cmd);

    tcpSocket.waitForReadyRead(500);

    rsp = tcpSocket.readAll();
    if (rsp.size()>0)
    {
        if (rsp.at(3)=='$')
            writeSuccess = true;
    }

#ifdef DEBUGLOGS_STAGE
    qDebug()<<"writePlcMem32 cmdStr: "<<cmdStr<<"  rsp:"<<rsp<<"writeSuccess"<<writeSuccess;
#endif

    return writeSuccess;
}

bool stageController::readPlcMem(short address,unsigned short * returnVal)
{
    QByteArray cmd;
    QByteArray rsp("No Response");
    QString addStrStart = "00"+QString::number(address);
    QString addStrEnd = "00"+QString::number(address);
    QString cmdStr = "%01#RDD"+addStrStart+addStrEnd+"**\r";
    cmd = cmdStr.toLocal8Bit();
    bool writeSuccess = false;
    unsigned short returnValBig=0;
    QTcpSocket tcpSocket;

    //if (tcpSocket.isOpen()==false || tcpSocket.isValid() ==false || tcpSocket.isWritable() == false)
        tcpSocket.connectToHost(HOSTIP, HOSTPORT);
    tcpSocket.write(cmd);

    tcpSocket.waitForReadyRead(500);

    rsp = tcpSocket.readAll();
    if (rsp.size()>0)
    {
        if (rsp.at(3)=='$')
        {
            writeSuccess = true;
            returnValBig = rsp.mid(6,4).toUShort(0,16);
            *returnVal = qFromBigEndian(returnValBig);
        }
    }
#ifdef DEBUGLOGS_STAGE
    qDebug()<<"readPlcMem cmdStr: "<<cmdStr<<"  rsp:"<<rsp<<"writeSuccess"<<writeSuccess<<"ReturnVal"<<*returnVal;
#endif
    return writeSuccess;
}

bool stageController::readPlcMem32(short address, unsigned int * returnVal)
{
    QByteArray cmd;
    QByteArray rsp("No Response");
    QString addStrStart = "00"+QString::number(address);
    QString addStrEnd = "00"+QString::number(address+1);

    QString cmdStr = "%01#RDD"+addStrStart+addStrEnd+"**\r";
    cmd = cmdStr.toLocal8Bit();
    bool writeSuccess = false;
    unsigned short returnValBigshortL=0,returnValshortL=0;
    unsigned short returnValBigshortH=0,returnValshortH=0;
    QString returnValBigshortLstring,returnValBigshortHstring;
    QTcpSocket tcpSocket;

    //if (tcpSocket.isOpen()==false || tcpSocket.isValid() ==false || tcpSocket.isWritable() == false)
        tcpSocket.connectToHost(HOSTIP, HOSTPORT);
    tcpSocket.write(cmd);

    tcpSocket.waitForReadyRead(500);

    rsp = tcpSocket.readAll();
    if (rsp.size()>0)
    {
        if (rsp.at(3)=='$')
        {
            writeSuccess = true;

            returnValBigshortL = rsp.mid(6,4).toUShort(0,16);
            returnValBigshortH = rsp.mid(10,4).toUShort(0,16);

            returnValshortL = qFromBigEndian(returnValBigshortL);
            returnValshortH = qFromBigEndian(returnValBigshortH);

            *returnVal = returnValshortL | (returnValshortH<<16);
        }
    }
#ifdef DEBUGLOGS_STAGE
    qDebug()<<"readPlcMem32 cmdStr: "<<cmdStr<<"  rsp:"<<rsp<<"writeSuccess"<<writeSuccess<<"ReturnVal"<<*returnVal;
#endif
    return writeSuccess;
}

void stageController::setErrorReset(void)
{
    writePlcMem(215,true); //x-origin
    QTimer::singleShot( 1000, this, SLOT(clearErrorReset() ));
}

void stageController::clearErrorReset(void)
{
    writePlcMem(215,false); //x-origin
}

void stageController::setServoStop(void)
{
    writePlcMem(216,true); //servo-stop
    QTimer::singleShot( 100, this, SLOT(clearServoStop() ));
}

void stageController::clearServoStop(void)
{
    writePlcMem(216,false); //servo-stop
}

void stageController::originSet(void)
{
    writePlcMem(221,true); //x-origin
    writePlcMem(222,true); //z-origin

    QTimer::singleShot( 1000, this, SLOT(originReset() ));
}

void stageController::originReset(void)
{
    writePlcMem(221,false); //x-origin
    writePlcMem(222,false); //z-origin
}

void stageController::updatePos()
{
    getPosX();
    getPosZ();
}

void stageController::JogzpStop()
{
    Jogzp(false);
}

void stageController::JogznStop()
{
    Jogzn(false);
}
void stageController::JogxpStop()
{
    Jogxp(false);
}

void stageController::JogxnStop()
{
    Jogxn(false);
}

void stageController::JogzpStart()
{
    Jogzp(true);
}

void stageController::JogznStart()
{
    Jogzn(true);
}
void stageController::JogxpStart()
{
    Jogxp(true);
}

void stageController::JogxnStart()
{
    Jogxn(true);
}

void stageController::Jogxp(bool enableJog)
{
    setErrorReset();
    if (writePlcMem(217,enableJog) == true && updateTimer->isActive()==false)
        updateTimer->start(500);
/*
    if (enableJog)
        updateTimer->start(100);
    else
    {
        updatePos();
        updateTimer->stop();
    }
*/
}

void stageController::Jogxn(bool enableJog)
{
    setErrorReset();
    if (writePlcMem(218,enableJog) == true && updateTimer->isActive()==false)
    updateTimer->start(500);

/*
    if (enableJog)
        updateTimer->start(100);
    else
    {
        updatePos();
        updateTimer->stop();
    }
    */
}

void stageController::Jogzp(bool enableJog)
{
    setErrorReset();
    if (writePlcMem(219,enableJog) == true && updateTimer->isActive()==false)
    updateTimer->start(500);
/*
    if (enableJog)
        updateTimer->start(100);
    else
    {
        updatePos();
        updateTimer->stop();
    }
*/
}

void stageController::Jogzn(bool enableJog)
{
    setErrorReset();
    if (writePlcMem(220,enableJog) == true && updateTimer->isActive()==false)
    updateTimer->start(500);
/*
    if (enableJog)
        updateTimer->start(100);
    else
    {
        updatePos();
        updateTimer->stop();
    }
*/
}

void stageController::setPosZ(unsigned int pos,bool relToScanStart)
{
    if (relToScanStart && this->scanStartPosZ > 0 )
        pos = pos + this->scanStartPosZ;
    writePlcMem32(230,pos);
    setMovetoZ();
}

void stageController::setMovetoZ()
{
    setErrorReset();
    writePlcMem(234,true);
    QTimer::singleShot( 1000, this, SLOT(resetMovetoZ() ));
}

void stageController::resetMovetoZ()
{
    writePlcMem(234,false);
}

void stageController::setPosX(unsigned int pos, bool relToScanStart)
{
    if (relToScanStart && this->scanStartPosX > 0 )
        pos = pos + this->scanStartPosX;

    writePlcMem32(232,pos);
    setMovetoX();
}

void stageController::setMovetoX()
{
    setErrorReset();
    writePlcMem(235,true);
    QTimer::singleShot( 1000, this, SLOT(resetMovetoX() ));
}

void stageController::resetMovetoX()
{
    writePlcMem(235,false);
}
unsigned int stageController::getPosZ()
{
    unsigned int returnVal;
    double returnValdouble;
    QString returnValString;

    if (readPlcMem32(302,&returnVal) == false)
        returnVal = 0;

    returnValdouble = (double)returnVal/1000;
    returnValdouble = round(returnValdouble*100)/100;
    returnValString = QString::number(returnValdouble,'f',2);

    //returnValString = QString::number(123.5,'f',2);

    emit Zpos(returnValString);
    return (returnVal);
}

unsigned int stageController::getPosX()
{
    unsigned int returnVal;
    double returnValdouble;
    QString returnValString;

    if (readPlcMem32(304,&returnVal) == false)
        returnVal = 0;

    returnValdouble = (double)returnVal/1000;
    returnValdouble = round(returnValdouble*100)/100;

    returnValString = QString::number(returnValdouble,'f',2);
    emit Xpos(returnValString);
    return (returnVal);
}

bool stageController::markScanArea(float height, float width, float interval)
{
    float HoverI = (unsigned short)height/interval + 1;

    //multiply with constants
    interval *= 1000;
    width *= 1000;
    unsigned int returnVal;

    qDebug("markScanArea - startScan: 208-interval: %f, 202-width: %f, 224-Height/interval:%f ",interval,width,HoverI);

    writePlcMem32(208,interval); //multiply by 1000
    writePlcMem32(202,width); //multiply by 1000
    writePlcMem(224,HoverI);

    readPlcMem32(302,&returnVal);//posx
    writePlcMem32(200,returnVal); //X-axis start position

    readPlcMem32(304,&returnVal);//posz
    writePlcMem32(206,returnVal); //Z-axis start position

    writePlcMem32(246,10000); //X-axis speed
    writePlcMem32(248,10000); //Z-axis speed

    writePlcMem(250,1); //start marking the scan area
    QTimer::singleShot( 1000, this, SLOT(clearScanAreaMarker() ));

    return true;
}

void stageController::clearScanAreaMarker()
{
    writePlcMem(250,0); //start marking the scan area
}

bool stageController::isReadyForRptScan()
{
    return(scanStartPosX == getPosX() && scanStartPosZ == getPosZ() );
}

bool stageController::startScan(float height, float width, float PRF, float interval, bool startAtCurPos)
{
    //setErrorReset();
    float HoverI = (float)height/interval + 1;
    short RevPerTrig = 100/interval;
    float speed = PRF*interval;
    int offset = 1500;
    qDebug("startScan: Height: %f, 202-width: %f, 204-speed: %f, 224-Height/interval:%f, 236-RevPerTrig:%d ",
           height,width,speed,HoverI,RevPerTrig);
/* acc time:K200
    if (speed >= 250)       offset = 12220;
    else if (speed == 200)  offset = 9880;
    else if (speed == 150)  offset = 7050;
    else if (speed == 125)  offset = 0;
    else if (speed <= 100)  offset = 2850;
*/

    //acc time:K150 mini stage

       if (speed == 100 && scanInfoPtr->enableTT == false)      offset = 3125; //2550 //2870
       if (speed == 250 && scanInfoPtr->enableTT == false)      offset = 2530; //7325 big //tt configuration: 250 mm/s -> offset: 6250   //7475
       if (speed == 250 && scanInfoPtr->enableTT == true)      offset = 2530; //7325 big //tt configuration: 250 mm/s -> offset: 6250   //7475
       if (speed == 150 && scanInfoPtr->enableTT == false)      offset = 2530;
       if (speed == 150 && scanInfoPtr->enableTT == true)      offset = 4500; //3500
       if (speed == 350 && scanInfoPtr->enableTT == false)      offset = 2000;
       if (speed == 400 && scanInfoPtr->enableTT == false)      offset = 2500;
       if (speed == 100 && scanInfoPtr->enableTT == true)      offset = 2550;  //tt configuration: 250 mm/s -> offset: 6250
       if (speed ==  75 && scanInfoPtr->enableTT == true)      offset = 2550;  //tt configuration: 250 mm/s -> offset: 6250
       if (speed ==  75 && scanInfoPtr->enableTT == false)      offset = 2550;  //tt configuration: 250 mm/s -> offset: 6250
     //acc time:K500 mini stage
        //if (speed == 100 && scanInfoPtr->enableTT == false)      offset = 2800;
        //if (speed == 250 && scanInfoPtr->enableTT == false)      offset = 8250; //7325 big //tt configuration: 250 mm/s -> offset: 6250   //7475
        //if (speed == 250 && scanInfoPtr->enableTT == true)       offset = 6250;  //tt configuration: 250 mm/s -> offset: 6250
        //if (speed == 100 && scanInfoPtr->enableTT == true)      offset = 2550;  //tt configuration: 250 mm/s -> offset: 6250
        //if (speed == 450 && scanInfoPtr->enableTT == true)      offset = 11000;  //tt configuration: 250 mm/s -> offset: 6250
        //if (speed == 350)       offset = 24730;
        //if (speed == 250)  offset = 13590; //15200
        //if (speed == 250 && width < 180)  offset = 13890;
        //if (speed == 250 && width < 100)  offset = 13690;
        //if (speed == 250 && width < 70)  offset = 13590;
        //if (speed == 250 && width == 90)  offset = 13290;
        //else if (speed < 250)   offset = 14725;
        //else if (speed == 200)  offset = 10100;//10100
        //if (speed == 200 && width < 150)  offset = 10400;
        //if (speed == 200 && width == 150)  offset = 10300;
        //else if (speed == 150)  offset = 7700;

        //else if (speed <= 100)  offset = 2850;


    //multiply with constants
    interval *= 1000;
    width *= 1000;
    speed *=100;
    qDebug("After multi - startScan: 208-interval: %f, 202-width: %f, 204-speed: %f, 224-Height/interval:%f, 236-RevPerTrig:%d, 238-240 - Offset:%d ",interval,width,speed,HoverI,RevPerTrig,offset);

    //first write the offset direction
    writePlcMem32(238,offset);
    writePlcMem32(240,offset);

    //multiply by 100
    writePlcMem32(208,interval); //multiply by 1000
    writePlcMem32(202,width); //multiply by 1000
    writePlcMem32(204,(unsigned int)speed); //speed multiply by 100
    writePlcMem32(210,2000); //Fixed Z-axis speed 1mm/s
    if (startAtCurPos)
    {
        scanStartPosX = getPosX();
        scanStartPosZ = getPosZ();
    }
    else
    {
        writePlcMem32(200,(int)(scanStartPosX)); //X-axis start position
        writePlcMem32(206,(int)(scanStartPosZ)); //Z-axis start position
    }
    writePlcMem32(254,5000); //acc distance
    writePlcMem(224,HoverI);
    writePlcMem(225,1); // repeat scan time
    writePlcMem(236,RevPerTrig);
    writePlcMem(251,0); // legacy scan mode
    writePlcMem(223,1); // start scan

    return true;
}


void stageController::gotoScanStart()
{
    setPosX(this->scanStartPosX);
    setPosZ(this->scanStartPosZ);
}
