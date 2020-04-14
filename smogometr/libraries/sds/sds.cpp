#include "Arduino.h"
#include "SDS.h"

SDS::SDS(Stream& stream)
{
  this->_stream = &stream;
}

// Standby mode. For low power consumption and prolong the life of the sensor.
void SDS::sleep()
{
  uint8_t command[] = { 0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB };
  _stream->write(command, sizeof(command));
}

// Operating mode. Stable data should be got at least 30 seconds after the sensor wakeup from the sleep mode because of the fan's performance.
void SDS::wakeUp()
{
  uint8_t command[] = { 0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0xAB };
  _stream->write(command, sizeof(command));
}

// Active mode. Default mode after power up. In this mode sensor would send serial data to the host automatically.
void SDS::activeMode()
{
  uint8_t command[] = { 0xAA, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0xAB };
  _stream->write(command, sizeof(command));
  _mode = MODE_ACTIVE;
}

// Passive mode. In this mode sensor would send serial data to the host only for request.
void SDS::passiveMode()
{
  uint8_t command[] = { 0xAA, 0xB4, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0xAB };
  _stream->write(command, sizeof(command));
  _mode = MODE_PASSIVE;
}

// Request read in Passive Mode.
void SDS::requestRead()
{
  if (_mode == MODE_PASSIVE)
  {
    uint8_t command[] = { 0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0xAB };
    _stream->write(command, sizeof(command));
  }
}

// Non-blocking function for parse response.
bool SDS::read(DATA& data)
{
  _data = &data;
  loop();
  
  return _status == STATUS_OK;
}

// Blocking function for parse response. Default timeout is 1s.
bool SDS::readUntil(DATA& data, uint16_t timeout)
{
  _data = &data;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) break;
  } while (millis() - start < timeout);

  return _status == STATUS_OK;
}

void SDS::loop()
{
  _status = STATUS_WAITING;
  if (_stream->available())
  {
    uint8_t ch = _stream->read();

    switch (_index)
    {
    case 0:
      if (ch != 0xAA)
      {
        return;
      }
      _calculatedChecksum = 0;
      break;

    case 1:
      if (ch != 0xC0)
      {
        _index = 0;
        return;
      }
      break;

    default:
      if (_index == 8)
      {
        _checksum = ch;
      }
      else if (_index == 9)
      {
        if (ch != 0xAB)
        {
          _index = 0;
          return;
        }
      
        if (_calculatedChecksum == _checksum)
        {
          _status = STATUS_OK;

          _data->pm_25 = makeWord(_payload[1], _payload[0]);
          _data->pm_100 = makeWord(_payload[3], _payload[2]);
          _data->dev_id = makeWord(_payload[5], _payload[4]);
        }

        _index = 0;
        return;
      }
      else
      {
        _calculatedChecksum += ch;
        uint8_t payloadIndex = _index - 2;

        if (payloadIndex < sizeof(_payload))
        {
          _payload[payloadIndex] = ch;
        }
      }

      break;
    }

    _index++;
  }
}
