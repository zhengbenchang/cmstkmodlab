#include <QApplication>

#include <nqlogger.h>

#include "NanotecSMCI36Model.h"

NanotecSMCI36Model::NanotecSMCI36Model(const char* port,
                                       double updateInterval1,
                                       double updateInterval2,
                                       QObject * /*parent*/)
  : QObject(),
    AbstractDeviceModel<NanotecSMCI36_t>(),
    NanotecSMCI36_PORT(port),
    updateInterval1_(updateInterval1),
    updateInterval2_(updateInterval2),
    driveAddress_(0)
{
  inputPinFunction_[0] = 0;
  outputPinFunction_[0] = 0;

  timer1_ = new QTimer(this);
  timer1_->setInterval(updateInterval1_ * 1000);
  connect( timer1_, SIGNAL(timeout()), this, SLOT(updateInformation1()) );

  timer2_ = new QTimer(this);
  timer2_->setInterval(updateInterval2_ * 1000);
  connect( timer2_, SIGNAL(timeout()), this, SLOT(updateInformation2()) );

  setDeviceEnabled(true);

  NQLog("NanotecSMCI36Model") << "constructed";
}

void NanotecSMCI36Model::setMotorID(int id)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setMotorID(" << id << ")";

  controller_->SetMotorID(id);

  updateInformation2();
}

void NanotecSMCI36Model::setPhaseCurrent(int current)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setPhaseCurrent(" << current << ")";

  controller_->SetPhaseCurrent(current);

  updateInformation2();
}

void NanotecSMCI36Model::setStandStillPhaseCurrent(int current)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setStandStillPhaseCurrent(" << current << ")";

  controller_->SetStandStillPhaseCurrent(current);

  updateInformation2();
}

void NanotecSMCI36Model::setStepMode(int mode)
{
  if (state_!=READY) return;

  controller_->SetStepMode(mode);

  updateInformation2();
}

const std::string NanotecSMCI36Model::getStepModeName() const
{
  if (state_!=READY) return std::string("unknown");
  return controller_->GetStepModeName(getStepMode());
}

const std::map<int,std::string>& NanotecSMCI36Model::getStepModeNames() const
{
  return controller_->GetStepModeNames();
}

void NanotecSMCI36Model::setErrorCorrectionMode(int mode)
{
  if (state_!=READY) return;

  controller_->SetErrorCorrectionMode(mode);

  updateInformation2();
}

const std::map<int,std::string>& NanotecSMCI36Model::getErrorCorrectionModeNames() const
{
  return controller_->GetErrorCorrectionModeNames();
}

void NanotecSMCI36Model::setRampMode(int mode)
{
  if (state_!=READY) return;

  controller_->SetRampMode(mode);

  updateInformation2();
}

const std::string NanotecSMCI36Model::getRampModeName() const
{
  if (state_!=READY) return std::string("unknown");
  return controller_->GetRampModeName(getRampMode());
}

const std::map<int,std::string>& NanotecSMCI36Model::getRampModeNames() const
{
  return controller_->GetRampModeNames();
}

void NanotecSMCI36Model::setPositioningMode(int mode)
{
  if (state_!=READY) return;


  controller_->SetPositioningMode(mode);

  updateInformation2();
}

const std::string NanotecSMCI36Model::getPositioningModeName() const
{
  if (state_!=READY) return std::string("unknown");
  return controller_->GetPositioningModeName(getPositioningMode());
}

const std::map<int,std::string>& NanotecSMCI36Model::getPositioningModeNames() const
{
  return controller_->GetPositioningModeNames();
}

void NanotecSMCI36Model::setMaxEncoderDeviation(int steps)
{
  if (state_!=READY) return;

  controller_->SetMaxEncoderDeviation(steps);

  updateInformation2();
}

void NanotecSMCI36Model::setDirection(bool direction)
{
  if (state_!=READY) return;


  controller_->SetDirection(direction);

  updateInformation2();
}

void NanotecSMCI36Model::setEncoderDirection(bool direction)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setEncoderDirection(" << direction << ")";

  controller_->SetEncoderDirection(direction);

  updateInformation2();
}

void NanotecSMCI36Model::setTravelDistance(int distance)
{
  if (state_!=READY) return;

  travelDistance_ = distance;

  controller_->SetTravelDistance(distance);

  updateInformation2();
}

void NanotecSMCI36Model::setMinFrequency(int frequency)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setMinFrequency(" << frequency << ")";

  controller_->SetMinimumFrequency(frequency);

  updateInformation2();
}

void NanotecSMCI36Model::setMaxFrequency(int frequency)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setMaxFrequency(" << frequency << ")";

  controller_->SetMaximumFrequency(frequency);

  updateInformation2();
}

void NanotecSMCI36Model::setMaxFrequency2(int frequency)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setMaxFrequency2(" << frequency << ")";

  controller_->SetMaximumFrequency2(frequency);

  updateInformation2();
}

void NanotecSMCI36Model::setQuickstopRampHzPerSecond(int ramp)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setQuickstopRampHzPerSecond(" << ramp << ")";

  controller_->SetQuickstopRampHzPerSecond(ramp);

  updateInformation2();
}

void NanotecSMCI36Model::setAccelerationRampHzPerSecond(int ramp)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setAccelerationRampHzPerSecond(" << ramp << ")";

  controller_->SetAccelerationRampHzPerSecond(ramp);

  updateInformation2();
}

void NanotecSMCI36Model::setDecelerationRampHzPerSecond(int ramp)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setDecelerationRampHzPerSecond(" << ramp << ")";

  controller_->SetDecelerationRampHzPerSecond(ramp);

  updateInformation2();
}

void NanotecSMCI36Model::start()
{
  if (state_!=READY) return;

  if (status_ & VNanotecSMCI36::smciReady) {
    controller_->Start();
  }
}

void NanotecSMCI36Model::stop()
{
  if (state_!=READY) return;

  controller_->Stop(false);
}

void NanotecSMCI36Model::quickStop()
{
  if (state_!=READY) return;

  controller_->Stop(true);
}

void NanotecSMCI36Model::resetPositionError()
{
  if (state_!=READY) return;

  controller_->ResetPositionError(controllerSteps_);
}

void NanotecSMCI36Model::setIOMask(unsigned int mask)
{
  if (state_!=READY) return;

  controller_->SetIOMask(mask);

  NQLogMessage("NanotecSMCI36Model") << "setIOMask(" << mask << ")";

  updateInformation2();
}

void NanotecSMCI36Model::setReversePolarityMask(unsigned int mask)
{
  if (state_!=READY) return;

  controller_->SetReversePolarityMask(mask);

  NQLogMessage("NanotecSMCI36Model") << "setReversePolarityMask(" << mask << ")";

  updateInformation2();
}

int NanotecSMCI36Model::getInputPinFunction(int pin) const
{
  return inputPinFunction_[pin];
}

const std::string NanotecSMCI36Model::getInputPinFunctionName(int pin) const
{
  if (state_!=READY) return std::string("unknown");
  return controller_->GetInputPinFunctionName(getInputPinFunction(pin));
}

const std::map<int,std::string>& NanotecSMCI36Model::getInputPinFunctionNames() const
{
  return controller_->GetInputPinFunctionNames();
}

void NanotecSMCI36Model::setInputPinFunction(int pin, int function)
{
  if (state_!=READY) return;

  controller_->SetInputPinFunction(pin, function);

  updateInformation2();
}

bool NanotecSMCI36Model::getInputPolarity(int pin) const
{
  unsigned int bit = controller_->GetInputBitForPin(pin);
  if (bit==0) return false;

  return (ioPolarityMask_&bit);
}

void NanotecSMCI36Model::setInputPolarity(int pin, bool reverse)
{
  unsigned int bit = controller_->GetInputBitForPin(pin);
  if (bit==0) return;

  if (reverse) {
    if (!(ioPolarityMask_&bit)) {
      controller_->SetReversePolarityMask(ioPolarityMask_|bit);
      updateInformation2();
    }
  } else {
    if (ioPolarityMask_&bit) {
      controller_->SetReversePolarityMask(ioPolarityMask_ & ~bit);
      updateInformation2();
    }
  }
}

bool NanotecSMCI36Model::getInputPinState(int pin) const
{
  unsigned int bit = controller_->GetInputBitForPin(pin);
  if (bit==0) return false;

  return (io_&bit);
}

int NanotecSMCI36Model::getOutputPinFunction(int pin) const
{
  return outputPinFunction_[pin];
}

const std::string NanotecSMCI36Model::getOutputPinFunctionName(int pin) const
{
  if (state_!=READY) return std::string("unknown");
  return controller_->GetOutputPinFunctionName(getOutputPinFunction(pin));
}

const std::map<int,std::string>& NanotecSMCI36Model::getOutputPinFunctionNames() const
{
  return controller_->GetOutputPinFunctionNames();
}

void NanotecSMCI36Model::setOutputPinFunction(int pin, int function)
{
  if (state_!=READY) return;

  controller_->SetOutputPinFunction(pin, function);

  updateInformation2();
}

bool NanotecSMCI36Model::getOutputPolarity(int pin) const
{
  unsigned int bit = controller_->GetOutputBitForPin(pin);
  if (bit==0) return false;

  return (ioPolarityMask_&bit);
}

void NanotecSMCI36Model::setOutputPolarity(int pin, bool reverse)
{
  unsigned int bit = controller_->GetOutputBitForPin(pin);
  if (bit==0) return;

  if (reverse) {
    if (!(ioPolarityMask_&bit)) {
      controller_->SetReversePolarityMask(ioPolarityMask_|bit);
      updateInformation2();
    }
  } else {
    if (ioPolarityMask_&bit) {
      controller_->SetReversePolarityMask(ioPolarityMask_ & ~bit);
      updateInformation2();
    }
  }
}

bool NanotecSMCI36Model::getOutputPinState(int pin) const
{
  unsigned int bit = controller_->GetOutputBitForPin(pin);
  if (bit==0) return false;

  return (io_&bit);
}

void NanotecSMCI36Model::setIO(unsigned int mask)
{
  if (state_!=READY) return;

  NQLogMessage("NanotecSMCI36Model") << "setIO(" << mask << ")";

  if (io_ != mask) {
    controller_->SetIO(mask);
    io_ = mask;

    emit informationChanged();
  }
}

void NanotecSMCI36Model::setOutputPinState(int pin, bool state)
{
  unsigned int bit = controller_->GetOutputBitForPin(pin);
  if (bit==0) return;

  if (state) {
    if (!(io_&bit)) {
      controller_->SetIO(io_ | bit);
      updateInformation1();
    }
  } else {
    if (io_&bit) {
      controller_->SetIO(io_ & ~bit);
      updateInformation1();
    }
  }
}

void NanotecSMCI36Model::initialize()
{
  NQLog("NanotecSMCI36Model") << "initialize() " << NanotecSMCI36_PORT;

  setDeviceState(INITIALIZING);

  renewController(NanotecSMCI36_PORT);

  bool enabled = ( controller_ != NULL ) && ( controller_->DeviceAvailable() );

  if ( enabled ) {
    setDeviceState(READY);
    updateInformation1();
    updateInformation2();
  }
  else {
    setDeviceState( OFF );
    delete controller_;
    controller_ = NULL;
  }
}

/// \see AbstractDeviceModel::setDeviceState
void NanotecSMCI36Model::setDeviceState( State state )
{
  if ( state_ != state ) {
    state_ = state;

    // No need to run the timer if the chiller is not ready
    if ( state_ == READY ) {
      timer1_->start();
      timer2_->start();
    } else {
      timer1_->stop();
      timer2_->stop();
    }

    emit deviceStateChanged(state);
  }
}

void NanotecSMCI36Model::updateInformation1()
{
  // NQLog("NanotecSMCI36Model", NQLog::Debug) << "updateInformation()";

  if (thread()==QApplication::instance()->thread()) {
    // NQLog("NanotecSMCI36Model", NQLog::Debug) << " running in main application thread";
  } else {
    // NQLog("NanotecSMCI36Model", NQLog::Debug) << " running in dedicated DAQ thread";
  }

  if ( state_ == READY ) {

    unsigned int status = controller_->GetStatus();
    int controllerSteps = controller_->GetPosition();
    int encoderSteps = controller_->GetEncoderPosition();

    unsigned int io = controller_->GetIO();

    if (status != status_ ||
        controllerSteps != controllerSteps_ ||
        encoderSteps != encoderSteps_ ||
        io != io_) {

      status_ = status;
      controllerSteps_ = controllerSteps;
      encoderSteps_ = encoderSteps;

      io_ = io;

      // NQLog("NanotecSMCI36Model", NQLog::Spam) << "information changed";

      emit informationChanged();
    }
  }
}

void NanotecSMCI36Model::updateInformation2()
{
  // NQLog("NanotecSMCI36Model", NQLog::Debug) << "updateInformation()";

  if (thread()==QApplication::instance()->thread()) {
    // NQLog("NanotecSMCI36Model", NQLog::Debug) << " running in main application thread";
  } else {
    // NQLog("NanotecSMCI36Model", NQLog::Debug) << " running in dedicated DAQ thread";
  }

  if ( state_ == READY ) {

    int driveAddress = controller_->GetDriveAddress();
    int motorID = controller_->GetMotorID();
    int phaseCurrent = controller_->GetPhaseCurrent();
    int standStillPhaseCurrent = controller_->GetStandStillPhaseCurrent();
    int stepMode = controller_->GetStepMode();
    int rampMode = controller_->GetRampMode();
    int positioningMode = controller_->GetPositioningMode();
    int errorCorrectionMode = controller_->GetErrorCorrectionMode();
    int maxEncoderDeviation = controller_->GetMaxEncoderDeviation();
    bool direction = controller_->GetDirection();
    bool encoderDirection = controller_->GetEncoderDirection();
    int travelDistance = controller_->GetTravelDistance();
    int minFrequency = controller_->GetMinimumFrequency();
    int maxFrequency = controller_->GetMaximumFrequency();
    int maxFrequency2 = controller_->GetMaximumFrequency2();
    int quickstopRamp = controller_->GetQuickstopRampHzPerSecond();
    int accelRamp = controller_->GetAccelerationRampHzPerSecond();
    int decelRamp = controller_->GetDecelerationRampHzPerSecond();

    std::array<int,7> inputPinFunction;
    inputPinFunction[0] = 0;
    for (int i=1;i<7;++i) {
      inputPinFunction[i] = controller_->GetInputPinFunction(i);
    }

    std::array<int,4> outputPinFunction;
    outputPinFunction[0] = 0;
    for (int i=1;i<4;++i) {
      outputPinFunction[i] = controller_->GetOutputPinFunction(i);
    }

    unsigned int ioMask = controller_->GetIOMask();
    unsigned int ioPolarityMask = controller_->GetReversePolarityMask();

    if (driveAddress != driveAddress_ ||
        motorID != motorID_ ||
        phaseCurrent != phaseCurrent_ ||
        standStillPhaseCurrent != standStillPhaseCurrent_ ||
        stepMode != stepMode_ ||
        rampMode != rampMode_ ||
        positioningMode != positioningMode_ ||
        errorCorrectionMode != errorCorrectionMode_ ||
        maxEncoderDeviation != maxEncoderDeviation_ ||
        direction != direction_ ||
        encoderDirection != encoderDirection_ ||
        travelDistance != travelDistance_ ||
        minFrequency != minFrequency_ ||
        maxFrequency != maxFrequency_ ||
        maxFrequency2 != maxFrequency2_ ||
        quickstopRamp != quickstopRamp_ ||
        accelRamp != accelRamp_ ||
        decelRamp != decelRamp_ ||
        inputPinFunction != inputPinFunction_ ||
        outputPinFunction != outputPinFunction_ ||
        ioMask != ioMask_ ||
        ioPolarityMask != ioPolarityMask_) {

      driveAddress_ = driveAddress;
      motorID_ = motorID;
      phaseCurrent_ = phaseCurrent;
      standStillPhaseCurrent_ = standStillPhaseCurrent;
      stepMode_ = stepMode;
      rampMode_ = rampMode;
      positioningMode_ = positioningMode;
      errorCorrectionMode_ = errorCorrectionMode;
      maxEncoderDeviation_ = maxEncoderDeviation;
      direction_ = direction;
      encoderDirection_ = encoderDirection;
      travelDistance_ = travelDistance;
      minFrequency_ = minFrequency;
      maxFrequency_ = maxFrequency;
      maxFrequency2_ = maxFrequency2;
      quickstopRamp_ = quickstopRamp;
      accelRamp_ = accelRamp;
      decelRamp_= decelRamp;

      inputPinFunction_ = inputPinFunction;
      outputPinFunction_ = outputPinFunction;
      ioMask_ = ioMask;
      ioPolarityMask_ = ioPolarityMask;

      // NQLog("NanotecSMCI36Model", NQLog::Spam) << "information changed";

      emit informationChanged();
    }
  }
}

/// Attempts to enable/disable the (communication with) the NanotecSMCI36 controller.
void NanotecSMCI36Model::setDeviceEnabled(bool enabled)
{
  AbstractDeviceModel<NanotecSMCI36_t>::setDeviceEnabled(enabled);
}

void NanotecSMCI36Model::setControlsEnabled(bool enabled)
{
  emit controlStateChanged(enabled);
}
