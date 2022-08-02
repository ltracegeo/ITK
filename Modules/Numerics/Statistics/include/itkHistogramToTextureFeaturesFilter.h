/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkHistogramToTextureFeaturesFilter_h
#define itkHistogramToTextureFeaturesFilter_h

#include "itkHistogram.h"
#include "itkMacro.h"
#include "itkProcessObject.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk
{
namespace Statistics
{
/**\class HistogramToTextureFeaturesFilterEnums
 *\brief This class contains all enum classes used by HistogramToTextureFeaturesFilter class.
 * \ingroup ITKStatistics
 */
class HistogramToTextureFeaturesFilterEnums
{
public:
  /**
   * \class TextureFeature
   * \ingroup ITKStatistics
   * Texture feature types
   */
  enum class TextureFeature : uint8_t
  {
    Energy,
    Entropy,
    Correlation,
    InverseDifferenceMoment,
    Inertia,
    ClusterShade,
    ClusterProminence,
    HaralickCorrelation,
    InvalidFeatureName
  };
};
// Define how to print enumeration
extern ITKStatistics_EXPORT std::ostream &
                            operator<<(std::ostream & out, const HistogramToTextureFeaturesFilterEnums::TextureFeature value);

/**
 *\class HistogramToTextureFeaturesFilter
 *  \brief This class computes texture feature coefficients from a grey level
 * co-occurrence matrix.
 *
 * This class computes features that summarize image texture, given a grey level
 * co-occurrence matrix (generated by a ScalarImageToCooccurrenceMatrixFilter
 * or related class).
 *
 * The features calculated are as follows (where \f$ g(i, j) \f$ is the element in
 * cell i, j of a normalized GLCM):
 *
 * "Energy" \f$ = f_1 = \sum_{i,j}g(i, j)^2 \f$
 *
 * "Entropy" \f$ = f_2 = -\sum_{i,j}g(i, j) \log_2 g(i, j)\f$, or 0 if \f$g(i, j) = 0\f$
 *
 * "Correlation" \f$ = f_3 = \sum_{i,j}\frac{(i - \mu)(j - \mu)g(i, j)}{\sigma^2} \f$
 *
 * "Difference Moment" \f$= f_4 = \sum_{i,j}\frac{1}{1 + (i - j)^2}g(i, j) \f$
 *
 * "Inertia" \f$ = f_5 = \sum_{i,j}(i - j)^2g(i, j) \f$ (sometimes called "contrast.")
 *
 * "Cluster Shade" \f$ = f_6 = \sum_{i,j}((i - \mu) + (j - \mu))^3 g(i, j) \f$
 *
 * "Cluster Prominence" \f$ = f_7 = \sum_{i,j}((i - \mu) + (j - \mu))^4 g(i, j) \f$
 *
 * "Haralick's Correlation" \f$ = f_8 = \frac{\sum_{i,j}(i, j) g(i, j) -\mu_t^2}{\sigma_t^2} \f$
 * where \f$\mu_t\f$ and \f$\sigma_t\f$ are the mean and standard deviation of the row
 * (or column, due to symmetry) sums.
 *
 * Above, \f$ \mu =  \f$ (weighted pixel average) \f$ = \sum_{i,j}i \cdot g(i, j) =
 * \sum_{i,j}j \cdot g(i, j) \f$ (due to matrix symmetry), and
 *
 * \f$ \sigma =  \f$ (weighted pixel variance) \f$ = \sum_{i,j}(i - \mu)^2 \cdot g(i, j) =
 * \sum_{i,j}(j - \mu)^2 \cdot g(i, j)  \f$  (due to matrix symmetry)
 *
 * A good texture feature set to use is the Conners, Trivedi and Harlow set:
 * features 1, 2, 4, 5, 6, and 7. There is some correlation between the various
 * features, so using all of them at the same time is not necessarily a good idea.
 *
 * NOTA BENE: The input histogram will be forcibly normalized!
 * This algorithm takes three passes through the input
 * histogram if the histogram was already normalized, and four if not.
 *
 * Web references:
 *
 * http://www.cssip.uq.edu.au/meastex/www/algs/algs/algs.html
 * https://www.ucalgary.ca/~mhallbey/texture/texture_tutorial.html
 *
 * Print references:
 *
 * Haralick, R.M., K. Shanmugam and I. Dinstein. 1973.  Textural Features for
 * Image Classification. IEEE Transactions on Systems, Man and Cybernetics.
 * SMC-3(6):610-620.
 *
 * Haralick, R.M. 1979. Statistical and Structural Approaches to Texture.
 * Proceedings of the IEEE, 67:786-804.
 *
 * R.W. Conners and C.A. Harlow. A Theoretical Comparison of Texture Algorithms.
 * IEEE Transactions on Pattern Analysis and Machine Intelligence,  2:204-222, 1980.
 *
 * R.W. Conners, M.M. Trivedi, and C.A. Harlow. Segmentation of a High-Resolution
 * Urban Scene using Texture  Operators. Computer Vision, Graphics and Image
 * Processing, 25:273-310,  1984.
 *
 * \sa ScalarImageToCooccurrenceMatrixFilter
 * \sa ScalarImageToTextureFeaturesFilter
 *
 * Author: Zachary Pincus
 * \ingroup ITKStatistics
 */

template <typename THistogram>
class ITK_TEMPLATE_EXPORT HistogramToTextureFeaturesFilter : public ProcessObject
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(HistogramToTextureFeaturesFilter);

  /** Standard type alias */
  using Self = HistogramToTextureFeaturesFilter;
  using Superclass = ProcessObject;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Run-time type information (and related methods). */
  itkTypeMacro(HistogramToTextureFeaturesFilter, ProcessObject);

  /** standard New() method support */
  itkNewMacro(Self);

  using HistogramType = THistogram;
  using HistogramPointer = typename HistogramType::Pointer;
  using HistogramConstPointer = typename HistogramType::ConstPointer;
  using MeasurementType = typename HistogramType::MeasurementType;
  using MeasurementVectorType = typename HistogramType::MeasurementVectorType;
  using IndexType = typename HistogramType::IndexType;
  using AbsoluteFrequencyType = typename HistogramType::AbsoluteFrequencyType;
  using RelativeFrequencyType = typename HistogramType::RelativeFrequencyType;

  using TotalAbsoluteFrequencyType = typename HistogramType::TotalAbsoluteFrequencyType;

  using TotalRelativeFrequencyType = typename HistogramType::TotalRelativeFrequencyType;

  /** Container to hold relative frequencies of the histogram */
  using RelativeFrequencyContainerType = std::vector<RelativeFrequencyType>;

  /** Method to Set/Get the input Histogram */
  using Superclass::SetInput;
  void
  SetInput(const HistogramType * histogram);

  const HistogramType *
  GetInput() const;

  /** Smart Pointer type to a DataObject. */
  using DataObjectPointer = DataObject::Pointer;

  /** Type of DataObjects used for scalar outputs */
  using MeasurementObjectType = SimpleDataObjectDecorator<MeasurementType>;

  /** Return energy texture value. */
  MeasurementType
  GetEnergy() const;

  const MeasurementObjectType *
  GetEnergyOutput() const;

  /** Return entropy texture value. */
  MeasurementType
  GetEntropy() const;

  const MeasurementObjectType *
  GetEntropyOutput() const;

  /** return correlation texture value. */
  MeasurementType
  GetCorrelation() const;

  const MeasurementObjectType *
  GetCorrelationOutput() const;

  /** Return inverse difference moment texture value. */
  MeasurementType
  GetInverseDifferenceMoment() const;

  const MeasurementObjectType *
  GetInverseDifferenceMomentOutput() const;

  /** Return inertia texture value. */
  MeasurementType
  GetInertia() const;

  const MeasurementObjectType *
  GetInertiaOutput() const;

  /** Return cluster shade texture value. */
  MeasurementType
  GetClusterShade() const;

  const MeasurementObjectType *
  GetClusterShadeOutput() const;

  /** Return cluster prominence texture value. */
  MeasurementType
  GetClusterProminence() const;

  const MeasurementObjectType *
  GetClusterProminenceOutput() const;

  /** Return Haralick correlation texture value. */
  MeasurementType
  GetHaralickCorrelation() const;

  const MeasurementObjectType *
  GetHaralickCorrelationOutput() const;

  using TextureFeatureEnum = HistogramToTextureFeaturesFilterEnums::TextureFeature;
#if !defined(ITK_LEGACY_REMOVE)
  /**Exposes enums values for backwards compatibility*/
  static constexpr TextureFeatureEnum Energy = TextureFeatureEnum::Energy;
  static constexpr TextureFeatureEnum Entropy = TextureFeatureEnum::Entropy;
  static constexpr TextureFeatureEnum Correlation = TextureFeatureEnum::Correlation;
  static constexpr TextureFeatureEnum InverseDifferenceMoment = TextureFeatureEnum::InverseDifferenceMoment;
  static constexpr TextureFeatureEnum Inertia = TextureFeatureEnum::Inertia;
  static constexpr TextureFeatureEnum ClusterShade = TextureFeatureEnum::ClusterShade;
  static constexpr TextureFeatureEnum ClusterProminence = TextureFeatureEnum::ClusterProminence;
  static constexpr TextureFeatureEnum HaralickCorrelation = TextureFeatureEnum::HaralickCorrelation;
  static constexpr TextureFeatureEnum InvalidFeatureName = TextureFeatureEnum::InvalidFeatureName;
#endif

  /** convenience method to access the texture values */
  MeasurementType
  GetFeature(TextureFeatureEnum feature);

protected:
  HistogramToTextureFeaturesFilter();
  ~HistogramToTextureFeaturesFilter() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** Make a DataObject to be used for output output. */
  using DataObjectPointerArraySizeType = ProcessObject::DataObjectPointerArraySizeType;
  using Superclass::MakeOutput;
  DataObjectPointer MakeOutput(DataObjectPointerArraySizeType) override;

  void
  GenerateData() override;

private:
  void
  ComputeMeansAndVariances(double & pixelMean,
                           double & marginalMean,
                           double & marginalDevSquared,
                           double & pixelVariance);

  RelativeFrequencyContainerType m_RelativeFrequencyContainer;
};
} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkHistogramToTextureFeaturesFilter.hxx"
#endif

#endif
