#ifndef KAZU_YOLO_SAMPLE_CXX
#define KAZU_YOLO_SAMPLE_CXX

#include "yolo_play.h"

extern "C" {
#include "image.h"
#include "network.h"
#include "detection_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
}

#include "opencv2/highgui/highgui_c.h"

#include <vector>
//extern void test_yolo(char *cfgfile, char *weightfile, char *filename, float thresh);

/*
void yolo::run(std::string cfg, std::string weights, std::string img_file)
{}
*/
namespace larcv {

  void convert_yolo_detections(float *predictions, int classes, int num, int square, int side, int w, int h,
			       float thresh, float **probs, box *boxes, int only_objectness)
    
  {
    int i,j,n;
    //int per_cell = 5*num+classes;
    for (i = 0; i < side*side; ++i){
      int row = i / side;
      int col = i % side;
      for(n = 0; n < num; ++n){
	int index = i*num + n;
	int p_index = side*side*classes + i*num + n;
	float scale = predictions[p_index];
	int box_index = side*side*(classes + num) + (i*num + n)*4;
	boxes[index].x = (predictions[box_index + 0] + col) / side * w;
	boxes[index].y = (predictions[box_index + 1] + row) / side * h;
	boxes[index].w = pow(predictions[box_index + 2], (square?2:1)) * w;
	boxes[index].h = pow(predictions[box_index + 3], (square?2:1)) * h;
	for(j = 0; j < classes; ++j){
	  int class_index = i*classes;
	  float prob = scale*predictions[class_index+j];
	  probs[index][j] = (prob > thresh) ? prob : 0;
	}
	if(only_objectness){
	  probs[index][0] = scale;
	}
      }
    }
  }
  
  char *convert(const std::string & s)
  {
    char *pc = new char[s.size()+1];
    std::strcpy(pc, s.c_str());
    return pc;
  }
  
  void yolo_play::run(std::string in_cfg, std::string in_weights, std::string in_img)
  {
    std::vector<std::string> voc_names = {"aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car",
					  "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike",
					  "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"};
    std::vector<char*> voc_names_cptr;
    std::transform(voc_names.begin(), voc_names.end(), std::back_inserter(voc_names_cptr), convert);
    
    image voc_labels[20];
    
    for(int i = 0; i < 20; ++i){
      char buff[256];
      sprintf(buff, "data/labels/%s.png", voc_names[i].c_str());
      voc_labels[i] = load_image_color(buff, 0, 0);
    }
    
    float thresh = .2;
    std::vector<char> str_in_cfg(in_cfg.begin(),in_cfg.end()); str_in_cfg.push_back('\0');
    std::vector<char> str_in_weights(in_weights.begin(),in_weights.end()); str_in_weights.push_back('\0');
    std::vector<char> str_in_img(in_img.begin(),in_img.end()); str_in_img.push_back('\0');
    
    char* cfgfile = &str_in_cfg[0];
    char* weightfile = &str_in_weights[0];
    char* filename = &str_in_img[0];
    
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
      load_weights(&net, weightfile);
    }
    detection_layer l = net.layers[net.n-1];
    set_batch_network(&net, 1);
    srand(2222222);
    clock_t time;
    char buff[256];
    char *input = buff;
    int j;
    float nms=.5;
    box *boxes = (box*)(calloc(l.side*l.side*l.n, sizeof(box)));
    float **probs = (float**)(calloc(l.side*l.side*l.n, sizeof(float *)));
    for(j = 0; j < l.side*l.side*l.n; ++j) probs[j] = (float*)(calloc(l.classes, sizeof(float *)));
    while(1){
      if(filename){
	strncpy(input, filename, 256);
      } else {
	printf("Enter Image Path: ");
	fflush(stdout);
	input = fgets(input, 256, stdin);
	if(!input) return;
	strtok(input, "\n");
      }
      image im = load_image_color(input,0,0);
      image sized = resize_image(im, net.w, net.h);
      float *X = sized.data;
      time=clock();
      float *predictions = network_predict(net, X);
      printf("%s: Predicted in %f seconds.\n", input, sec(clock()-time));
      convert_yolo_detections(predictions, l.classes, l.n, l.sqrt, l.side, 1, 1, thresh, probs, boxes, 0);
      if (nms) do_nms_sort(boxes, probs, l.side*l.side*l.n, l.classes, nms);
      //draw_detections(im, l.side*l.side*l.n, thresh, boxes, probs, voc_names, voc_labels, 20);
      draw_detections(im, l.side*l.side*l.n, thresh, boxes, probs, &voc_names_cptr[0], 0, 20);
      show_image(im, "predictions");
      save_image(im, "predictions");
      
      show_image(sized, "resized");
      free_image(im);
      free_image(sized);
      cvWaitKey(0);
      cvDestroyAllWindows();
      if (filename) break;
    }
    
  }
}
#endif
