#include "pchABase.hpp"
#include "AMovingAverage.hpp"
#include "AString.hpp"
#include "AFile.hpp"
#include "AOutputBuffer.hpp"
#include "ANumber.hpp"
#include "AXmlElement.hpp"

void AMovingAverage::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AMovingAverage @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Average=" << m_Average;
  os << "  m_Count=" << m_Count;
  os << "  m_Weight=" << m_Weight;
  os << "  m_WeightAdjuster=" << m_WeightAdjuster << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_KeepSize=" << m_KeepSize;
  os << "  m_KeepPos=" << m_KeepPos << std::endl;
 
  //a_Keep buffer
  ADebugDumpable::indent(os, indent+1) << "mp_Keep={" << std::endl;
  ADebugDumpable::indent(os, indent+2);
  for (u4 i=0; i<m_KeepSize; ++i)
  {
    os << i << ":" << mp_Keep[i] << "  ";
    if (!((i+1) % 8))
    {
      os << std::endl;
      if (i < m_KeepSize-1)
        ADebugDumpable::indent(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AMovingAverage::AMovingAverage(
  u4 keepLastNSamples,   // = 16
  double weightAdjuster, // = 0.93
  double weightMinimum   // = 0.005
) :
  m_Weight(1.0),
  m_Average(0.0),
  m_Count(0),
  m_WeightAdjuster(weightAdjuster),
  m_WeightMinimum(weightMinimum),
  m_KeepSize(keepLastNSamples),
  m_KeepPos(0)
{
  AASSERT(this, weightAdjuster >= 0.0 && weightAdjuster <= 1.0);
  AASSERT(this, weightMinimum >= 0.0 && weightMinimum <= 1.0);

  if (!keepLastNSamples)
    m_KeepSize = 1;

  mp_Keep = new double[m_KeepSize];
  for (u4 i=0; i<m_KeepSize; ++i)
    mp_Keep[i] = 0.0;
}

AMovingAverage::~AMovingAverage()
{
  try
  {
    delete []mp_Keep;
  }
  catch(...) {}
}

void AMovingAverage::addSample(double dSample)
{
  m_Average = (1.0 - m_Weight) * m_Average + 
               m_Weight * dSample;
 
  //a_Now adjust the weigth
  if (m_Weight > m_WeightMinimum)
    m_Weight *= m_WeightAdjuster;
  else
    m_Weight = m_Weight;

  //a_Increment the count
  ++m_Count;

  //a_Add to keep
  mp_Keep[m_KeepPos++] = dSample;
  if (m_KeepPos >= m_KeepSize)
    m_KeepPos = 0;
}

void AMovingAverage::clear()
{
  m_Average = 0.0;
  m_Count   = 0;
  m_Weight  = 1.0;

  for (u4 i=0; i<m_KeepSize; ++i)
    mp_Keep[i] = 0.0;
}

void AMovingAverage::setWeight(double dWeight)
{
  if (dWeight < 0 || dWeight > 1.0)
    ATHROW(this, AException::InvalidParameter);

  m_Weight = dWeight;
}

void AMovingAverage::setWeightAdjuster(double dWeightAdjuster)
{
  if (dWeightAdjuster < 0 || dWeightAdjuster > 1.0)
    ATHROW(this, AException::InvalidParameter);

  m_WeightAdjuster = dWeightAdjuster;
}

void AMovingAverage::setWeightMinimum(double dWeightMinimum)
{
  if (dWeightMinimum < 0 || dWeightMinimum > 1.0)
    ATHROW(this, AException::InvalidParameter);

  m_WeightMinimum = dWeightMinimum;
}

void AMovingAverage::fromAFile(AFile& aFile)
{
  aFile.read(m_Average);
  aFile.read(m_Count);
  aFile.read(m_Weight);
  aFile.read(m_WeightAdjuster);
  aFile.read(m_WeightMinimum);
  aFile.read(m_KeepSize);
  aFile.read(m_KeepPos);
  delete []mp_Keep;
  mp_Keep = new double[m_KeepSize];
  for (u4 i=0; i<m_KeepSize; ++i)
    aFile.read(mp_Keep[i]);
}

void AMovingAverage::toAFile(AFile& aFile) const
{
  aFile.write(m_Average);
  aFile.write(m_Count);
  aFile.write(m_Weight);
  aFile.write(m_WeightAdjuster);
  aFile.write(m_WeightMinimum);
  aFile.write(m_KeepSize);
  aFile.write(m_KeepPos);
  for (u4 i=0; i<m_KeepSize; ++i)
    aFile.write(mp_Keep[i]);
}

void AMovingAverage::emit(AOutputBuffer& target) const
{
  target.append("average=");
  ANumber num(m_Average);
  num.setPrecision(4);
  num.emit(target);
  target.append("  count=");
  target.append(AString::fromU4(m_Count));
  if (mp_Keep)
  {
    target.append("  samples={");
    u4 i=0;
    while(i<m_KeepSize)
    {
      target.append(AString::fromDouble(mp_Keep[i]));
      ++i;
      if (i<m_KeepSize)
        target.append(",");
    }
    target.append("}");
  }
}

AXmlElement& AMovingAverage::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());
  
  thisRoot.addElement(ASW("average",7)).addData(m_Average);
  thisRoot.addElement(ASW("count",5)).addData(AString::fromU4(m_Count));

  AXmlElement& keep = thisRoot.addElement(ASW("keep",4));
  if (mp_Keep)
  {
    for (u4 i=0; i<m_KeepSize; ++i)
    {
      if (i == m_KeepPos)
        keep.addElement(ASW("sample",6)).addData(mp_Keep[i]).addAttribute(ASW("pos",3), AConstant::ASTRING_ONE);
      else
        keep.addElement(ASW("sample",6)).addData(mp_Keep[i]);
    }
  }

  return thisRoot;
}

double AMovingAverage::getAverage() const                 
{ 
  return m_Average; 
}

u8 AMovingAverage::getCount() const                   
{ 
  return m_Count; 
}

void AMovingAverage::setInitialAverage(double dInitial) 
{ 
  m_Average = dInitial; 
}
