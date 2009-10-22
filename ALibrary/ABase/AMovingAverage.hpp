/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AMovingAverage_HPP__
#define INCLUDED__AMovingAverage_HPP__

#include "apiABase.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"

class AFile;

/*!
This is meant to be used to get a rough average
 when the number of samples or value of the sum for the average
 may overflow the machine based numbers
 Depending on data fluctuation and weight parameters
 this class can track result fairly accurately
 It should not be used to determine exact results
 Results are repeatable and reproduceable with same data
*/
class ABASE_API AMovingAverage : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  keepLastNSamples - number of last samples to keep saved
  weightAdjuster - wieught is multiplied by adjuster every sample (samples are less important the more we get), initial weight is 1.0 and cannot be get than minimum
  weightMinimum - minimum value a sample weight will have (higher value will allow new samples to significantly alter the average, lower will harder the average against new data and slightly alter it)

  Algorithm for  void addSample(double SampleValue):
      Average = (1.0 - Weight) * Average + Weight * SampleValue;
      Weight = MIN(Weight * WeightAdjuster, WeightMinimum);
      ++Count;
      SaveSampleValue();
  */
  AMovingAverage(
    size_t keepBufferSize = 16, 
    double weightAdjuster = 0.92,
    double weightMinimum = 0.008
  );
  virtual ~AMovingAverage();
                    
  /*!
  Add a sample

  @param sampleValue to add to the average
  */
  void addSample(double sampleValue);
  
  /*!
  @return current average
  */
  double getAverage() const;
  
  /*!
  @return total samples proccessed
  */
  u8 getCount() const;
  
  /*!
  Set initial average value (by default 0.0)

  @param initial value
  */
  void setInitialAverage(double initial = 0.0);

  /*!
  Resize the keep buffer and discard all existing data

  @param new size
  */
  void setKeepBufferSize(size_t size);
  
  /*!
  Clear the data and average
  Set inital average to 0.0 and initial weight to 1.0
  Adjuster and minimum are not changed
  */
  void clear();

  /*!
  WeightAdjuster is used to reduce the Weigth as more samples are added (weight multiplied by adjuster every sample)
  */
  void setWeightAdjuster(double dAdjuster);  //a_[0.0, 1.0]

  /*!
  WeightMinimum is the lost value that the weight will get to (if it gets to 0.0 then at that point all new values are ignored)
  */
  void setWeightMinimum(double dMinimum);    //a_[0.0, 1.0]

  /*!
  Since weight is adjusted every sample, this is a reset-like function to adjust weight of the average to incoming samples
  Weight is used to adjust the importance of the current average (lower favors the old average)
    1.0 will ignore the average, incoming sample will overwrite average (equivalent to resetting the average)
    0.8 decent value, 80% of average + 20% of sample is the new average (useful for increasing value of incoming data)
    0.5 old average and new value are equally important and are averaged (useful for slightly increasing value of incoming data)
    0.0 will ignore all incoming values (equivalent to supressing incoming data)
  */
  void setWeight(double dWeight);            //a_[0.0, 1.0]

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  // no copy ctor
  AMovingAverage(const AMovingAverage&) {}

  // no assign operator
  AMovingAverage& operator =(const AMovingAverage&) { return *this; }

  // The average
  double m_Average;
  
  //a_Count of the samples
  u8 m_Count;

  //a_Weigth of the average when compared against incomming value;  Must be between 0.0 and 1.0, 
  double m_Weight;		
  double m_WeightAdjuster;
  double m_WeightMinimum;
  
  //a_Store last N samples in a rotating array
  double *mp_Keep;
  size_t  m_KeepPos;
  size_t  m_KeepSize;
};

#endif //INCLUDED__AMovingAverage_HPP__
