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
  weightAdjuster - sample weight of is multiplied by this every sample (samples are less important the more we get)
  weightMinimum - minimum value a sample weight will have (higher value will allow new samples to significantly alter the average, lower will harder the average against new data and slightly alter it)
  */
  AMovingAverage(
    u4 keepLastNSamples = 16, 
    double weightAdjuster = 0.92,
    double weightMinimum = 0.008
  );
  virtual ~AMovingAverage();
                    
  /*!
  Access methods
  */
  void   addSample(double dSample);
  double getAverage() const;
  u8     getCount() const;
  void   setInitialAverage(double dInitial);
  void   clear();                              //a_The parameters do NOT get cleared only data

  /*!
  WeightAdjuster is used to reduce the Weigth as more samples are added (weight multiplied by adjuster every sample)
  WeightMinimum is the lost value that the weight will get to (if it gets to 0.0 then at that point all new values are ignored)
  */
  void setWeightAdjuster(double dAdjuster);  //a_[0.0, 1.0]
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
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  double m_Average;
  
  //a_Count of the samples
  u8 m_Count;

  //a_Weigth of the average when compared against incomming value;  Must be between 0 and 1, 
  double m_Weight;		
  double m_WeightAdjuster;
  double m_WeightMinimum;

  //a_Store last N samples in a rotating array
  double *mp_Keep;
  u4      m_KeepPos;
  u4      m_KeepSize;
};

#endif //INCLUDED__AMovingAverage_HPP__
