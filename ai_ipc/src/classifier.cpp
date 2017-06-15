#include "classifier.h"
#include <QDebug>

using namespace cv;
using namespace caffe;
using std::string;


/* Pair (label, confidence) representing a prediction. */
typedef std::pair<string, float> Prediction;

const string modelFile = "../ai_ipc/3party/caffe/Model/deploy.prototxt";
const string trainedFile = "../ai_ipc/3party/caffe/Model/ajguard.caffemodel";
const string meanFile = "../ai_ipc/3party/caffe/Model/mean.binaryproto";
const string labelFile = "../ai_ipc/3party/caffe/Model/labels.txt";
const string imageFile = "../ai_ipc/test/test1000.jpg";

classifier::classifier(QObject *parent) : QObject(parent)
{
    Caffe::set_mode(Caffe::CPU);    // Caffe::set_mode(Caffe::GPU);

    // Load the network
    cnn.reset(new Net<float>(modelFile, TEST));
    cnn->CopyTrainedLayersFrom(trainedFile);

    // should be 1 or 3 channels
    Blob<float>* input_layer = cnn->input_blobs()[0];
    num_channels = input_layer->channels();

    input_geometry = cv::Size(input_layer->width(), input_layer->height());

    // Load the binaryproto mean file
    SetMean(meanFile);

    // Load labels
    std::ifstream label(labelFile.c_str());
    //CHECK(labels) << "Unable to open labels file " << label_file;

    string line;
    while( std::getline(label, line) )
    {
      labels.push_back(string(line));
    }

    Blob<float>* output_layer = cnn->output_blobs()[0];
    CHECK_EQ(labels.size(), output_layer->channels()) << "Number of labels is different from the output layer dimension.";
}

/********************************************************************************
*   Slot to accepte img from outside and do predict
********************************************************************************/
void classifier::classify(void)
{
    cv::Mat img = cv::imread( imageFile, -1 );

    std::vector<float> output = Predict(img);

    float tmp = 0;
    int idxTmp = 0;

    for (size_t i = 0; i < output.size(); ++i)
    {
        if(output[i] > tmp)
        {
            tmp = output[i];
            idxTmp = i;
        }
    }

    emit finishPredictSig( idxTmp, (int)(output[idxTmp] * 100) );
}

/* //This will use cmd line for predict, as a baseline of other method
void MainWindow::processImg( void )
{
    QImage img( path );

    calTime->restart();

    // Conver image to accepted size and format
    *inImg = inImg->fromImage(img.scaled(256,256,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QFile file("../ai_ipc/test/tmp.jpg");
    file.open(QIODevice::WriteOnly);
    inImg->save(&file, "jpeg",100);
    file.close();

    ui->label_img->setPixmap( *inImg );

    qDebug() << "com:" + caffeBin+param;

    // Call external exe to process detection and get result
    imgProcess->start( caffeBin+param, QIODevice::ReadWrite );
    imgProcess->waitForFinished( 5000 );

}
*/

/*******************************************************************************
*       Private functions
********************************************************************************/

/*******************************************************************************/
std::vector<float> classifier::Predict(const cv::Mat& img)
{
  Blob<float>* input_layer = cnn->input_blobs()[0];
  input_layer->Reshape(1, num_channels, input_geometry.height, input_geometry.width);

  /* Forward dimension change to all layers. */
  cnn->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(img, &input_channels);

  cnn->Forward();

  /* Copy the output layer to a std::vector */
  Blob<float>* output_layer = cnn->output_blobs()[0];
  const float* begin = output_layer->cpu_data();
  const float* end = begin + output_layer->channels();
  return std::vector<float>(begin, end);
}

/*******************************************************************************/
/* Load the mean file in binaryproto format. */
void classifier::SetMean(const string& mean_file)
{
  BlobProto blob_proto;
  ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

  /* Convert from BlobProto to Blob<float> */
  Blob<float> mean_blob;
  mean_blob.FromProto(blob_proto);
  CHECK_EQ(mean_blob.channels(), num_channels)
    << "Number of channels of mean file doesn't match input layer.";

  /* The format of the mean file is planar 32-bit float BGR or grayscale. */
  std::vector<cv::Mat> channels;
  float* data = mean_blob.mutable_cpu_data();
  for (int i = 0; i < num_channels; ++i) {
    /* Extract an individual channel. */
    cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
    channels.push_back(channel);
    data += mean_blob.height() * mean_blob.width();
  }

  /* Merge the separate channels into a single image. */
  cv::Mat mean;
  cv::merge(channels, mean);

  /* Compute the global mean pixel value and create a mean image
   * filled with this value. */
  cv::Scalar channel_mean = cv::mean(mean);
  mean = cv::Mat(input_geometry, mean.type(), channel_mean);
}

/*******************************************************************************/
/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void classifier::WrapInputLayer(std::vector<cv::Mat>* input_channels)
{
  Blob<float>* input_layer = cnn->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

/*******************************************************************************/
void classifier::Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels)
{
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry)
    cv::resize(sample, sample_resized, input_geometry);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == cnn->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}
