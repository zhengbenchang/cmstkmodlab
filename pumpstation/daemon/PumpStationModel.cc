#include <QApplication>

#include <ApplicationConfig.h>

#include <nqlogger.h>

#include "PumpStationModel.h"

PumpStationModel::PumpStationModel(ConradModel* conradModel,
                                   LeyboldGraphixThreeModel* leyboldModel,
                                   double updateInterval,
                                   QObject * /* parent */)
  : QObject(),
    conradModel_(conradModel),
    leyboldModel_(leyboldModel),
    conradDataValid_(false),
    leyboldDataValid_(false),
    dataValid_(false),
    updateInterval_(updateInterval)
{
	ApplicationConfig * config = ApplicationConfig::instance();

  pumpChannels_ = config->getValueVector<int>("PumpSwitches");
  valveChannels_ = config->getValueVector<int>("ValveSwitches");
  pumpOperatingHours_[1] = config->getValue<double>("Pump1OperatingHours");
  pumpOperatingHours_[2] = config->getValue<double>("Pump2OperatingHours");

  for (int i=0;i<5;++i) {
    switchBlocked_[i] = true;
    switchState_[i] = OFF;
  }

  for (int i=0;i<3;i++) {
    sensorStatus_[i] = LeyboldGraphixThree_t::SensorStatus_unknown;
    pressure_[i] = 0;
  }

  connect(leyboldModel_, SIGNAL(informationChanged()),
          this, SLOT(updateInformation()));

  timer_ = new QTimer(this);
  timer_->setInterval(updateInterval_ * 1000);
  connect(timer_, SIGNAL(timeout()), this, SLOT(updateConrad()));

  timer_->start();

  pump1timer_ = new QTimer(this);
  pump1timer_->setInterval(30 * 1000);
  connect(pump1timer_, SIGNAL(timeout()), this, SLOT(pump1HeartBeat()));

  pump2timer_ = new QTimer(this);
  pump2timer_->setInterval(30 * 1000);
  connect(pump2timer_, SIGNAL(timeout()), this, SLOT(pump2HeartBeat()));

  updateInformation();
  updateConrad();

  NQLog("PumpStationModel") << "constructed";
}

bool PumpStationModel::getSwitchBlocked(int channel) const
{
  return switchBlocked_[channel];
}

const State& PumpStationModel::getSwitchState( int channel ) const
{
  return switchState_[channel];
}

std::string PumpStationModel::getSensorName(int sensor) const
{
  return leyboldModel_->getSensorName(sensor);
}

double PumpStationModel::getPressure(int sensor) const
{
  if (sensor<1 || sensor>3) return -1.0;
  return pressure_[sensor-1];
}

int PumpStationModel::getSensorStatus(int sensor) const
{
  if (sensor<1 || sensor>3) return (int)LeyboldGraphixThree_t::SensorStatus_unknown;
  return (int)sensorStatus_[sensor-1];
}

void PumpStationModel::setSwitchBlocked(int channel, bool blocked)
{
  switchBlocked_[channel] = blocked;
}

void PumpStationModel::setSwitchEnabled(int channel, bool enabled)
{
  // if (switchBlocked_[channel]) return;

  conradModel_->setSwitchEnabled(channel, enabled);

  if (enabled) {
    if (switchState_[channel] != READY) {
      switchState_[channel] = READY;
      emit switchStateChanged(channel, switchState_[channel]);
    }
  } else {
    if (switchState_[channel] != OFF) {
      switchState_[channel] = OFF;
      emit switchStateChanged(channel, switchState_[channel]);
    }
  }
}

void PumpStationModel::updateInformation()
{
  // NQLog("PumpStationModel", NQLog::Message) << "updateInformation()";

  if (thread()==QApplication::instance()->thread()) {
    // NQLog("PumpStationModel", NQLog::Debug) << " running in main application thread";
  } else {
    // NQLog("PumpStationModel", NQLog::Debug) << " running in dedicated DAQ thread";
  }

  std::array<LeyboldGraphixThree_t::SensorStatus,3> sensorStatus;
  std::array<double,3> pressure;
  for (int i=0;i<3;++i) {

    sensorStatus[i] = leyboldModel_->getSensorStatus(i+1);
    if (sensorStatus_[i] != sensorStatus[i]) {
      sensorStatus_[i] = sensorStatus[i];
      emit sensorStatusChanged(i+1, (int)sensorStatus_[i]);
    }

    pressure[i] = leyboldModel_->getPressure(i+1);

    if (pressure_[i] != pressure[i]) {
      pressure_[i] = pressure[i];
      emit pressureChanged(i+1, pressure_[i]);
    }
  }

  leyboldDataValid_ = true;
  if (!dataValid_) {
    if (conradDataValid_) {
      dataValid_ = true;
      emit dataValid();
    }
  }
}

void PumpStationModel::updateConrad()
{
  // NQLog("PumpStationModel", NQLog::Message) << "updateConrad()";

  if (thread()==QApplication::instance()->thread()) {
    // NQLog("PumpStationModel", NQLog::Debug) << " running in main application thread";
  } else {
    // NQLog("PumpStationModel", NQLog::Debug) << " running in dedicated DAQ thread";
  }

  std::array<State,5> switchState;
  for (int i=0;i<5;++i) {
    switchState[i] = conradModel_->getSwitchState(i);
    if (switchState_[i] != switchState[i]) {

    	if (i==pumpChannels_[0]) { // pump 1
    		if (switchState[i]==OFF) {
    			pump1timer_->stop();
    		} else if (switchState[i]==READY) {
    			pump1timer_->start();
    		}
    	}

    	if (i==pumpChannels_[1]) { // pump 1
    		if (switchState[i]==OFF) {
    			pump2timer_->stop();
    		} else if (switchState[i]==READY) {
    			pump2timer_->start();
    		}
    	}

      emit switchStateChanged(i, switchState[i]);
    }
  }

  switchState_ = switchState;

  conradDataValid_ = true;
  if (!dataValid_) {
    if (leyboldDataValid_) {
      dataValid_ = true;
      emit dataValid();
    }
  }
}

double PumpStationModel::getPumpOperatingHours(int pump) const
{
	if (pump<1 || pump>2) return 0;
	return pumpOperatingHours_[pump];
}

void PumpStationModel::setPumpOperatingHours(int pump, double value)
{
	if (pump<1 || pump>2) return;
	pumpOperatingHours_[pump] = value;
}

void PumpStationModel::pump1HeartBeat()
{
	pumpOperatingHours_[1] += 30. / 3600.;

	ApplicationConfig * config = ApplicationConfig::instance();
	config->setValue<double>("Pump1OperatingHours", pumpOperatingHours_[1]);
}

void PumpStationModel::pump2HeartBeat()
{
	pumpOperatingHours_[2] += 30. / 3600.;

	ApplicationConfig * config = ApplicationConfig::instance();
	config->setValue<double>("Pump2OperatingHours", pumpOperatingHours_[2]);
}