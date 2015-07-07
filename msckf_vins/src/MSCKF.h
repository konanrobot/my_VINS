//
//  MSCKF.h
//  MyTriangulation
//
//  Created by Yang Shuo on 18/3/15.
//  Copyright (c) 2015 Yang Shuo. All rights reserved.
//

#ifndef __MyTriangulation__MSCKF__
#define __MyTriangulation__MSCKF__

#include <iostream>
#include <cmath>
#include <list>
#include <algorithm>
#include <vector>
#include <numeric>
#include <Eigen/Dense>
#include <Eigen/QR>

//#include <ros/ros.h>
//#include <ros/console.h>

using namespace Eigen;
using namespace std;

#include "FeatureRecord.h"
#include "Camera.h"

struct SlideState
{
    Vector4d q;
    Vector3d p;
    Vector3d v;
};

// chi-square test critical value table
const double chi_test[] = 
{
        0,          // 0
        0.004,  //  1
        0.103,  //  2
        0.352,
        0.711,
        1.145,
        1.635,
        2.167,
        2.733,
        3.325,
        3.940,
        4.575,
        5.226,
        5.892,
        6.571,
        7.261,
        7.962,
        8.672,
        9.390,
        10.117,
        10.851,
        11.591,
        12.338,
        13.091,
        13.848,
        14.611,
        15.379,
        16.151,
        16.928,
        17.708,
        18.493
};

class MSCKF
{
private:
    /* states */
//    VectorXd nominalState;  // dimension 4 + 3 + 3 + 3 + 3 = 16
//    VectorXd errorState;    // dimension 3 + 3 + 3 + 3 + 3 = 15
//    VectorXd extrinsicP;    // p_bc, dimension 3
    
    VectorXd fullNominalState;
//    VectorXd fullErrorState;
    
    list<SlideState> slidingWindow;

    /* covariance */
    MatrixXd errorCovariance;
    MatrixXd fullErrorCovariance;
    MatrixXd phi;
    
    /* noise matrix */
    MatrixXd Nc;
    double measure_noise;
    
    /* feature management */
    map<int, FeatureRecord> feature_record_dict;
    list<VectorXd>  residual_list;
    list<MatrixXd>  H_mtx_list;
    list<int>       H_mtx_block_size_list;
    
    
    double current_time;     // indicates the current time stamp
    int   current_frame;    // indicates the current frame in slidingWindow
    
    /* IMU measurements */
    Vector3d prev_w, curr_w;
    Vector3d prev_a, curr_a;
    
    /* nominal state variables used only for calculation */
    Vector4d spatial_quaternion; // q_gb
    Matrix3d spatial_rotation; // R_gb
    Vector3d spatial_position;
    Vector3d spatial_velocity;
    Vector3d gyro_bias;
    Vector3d acce_bias;
    

    
    /* fixed rotation between camera and the body frame */
    Matrix3d R_cb;
    

    
    void correctNominalState(VectorXd delta);
    void addSlideState();
    void removeSlideState(int index, int total);
    void addFeatures(const vector<pair<int, Vector3d>> &image);
    void removeFrameFeatures(int index);
    void removeUsedFeatures();
    
    Vector2d projectPoint(Vector3d feature_pose, Matrix3d R_bg, Vector3d p_gb, Vector3d p_cb);
    bool getResidualH(VectorXd& ri, MatrixXd& Hi, Vector3d feature_pose, MatrixXd measure, MatrixXd pose_mtx, int frame_offset);
    
public:
    /* camera */    
    DistortCamera cam;
    
    MSCKF();
    ~MSCKF();
    
    void resetError();
    
    void processIMU(double t, Vector3d linear_acceleration, Vector3d angular_velocity);
    void processImage(const vector<pair<int, Vector3d>> &image);
    
    void setNominalState(Vector4d q, Vector3d p, Vector3d v, Vector3d bg, Vector3d ba);
    void setCalibParam(Vector3d p_cb, double fx, double fy, double ox, double oy, double k1, double k2, double p1, double p2, double k3);
    void setIMUCameraRotation(Matrix3d _R_cb);
    
    void setNoiseMatrix(double dgc, double dac, double dwgc, double dwac);
    void setMeasureNoise(double _noise);

    void setErrorCovarianceIdentity();

    // test function...
    Vector2d projectCamPoint(Vector3d ptr);    

    /* outputs */
    Quaterniond getQuaternion();
    Matrix3d getRotation();
    Vector3d getPosition();
    Vector3d getVelocity();
    Vector3d getGyroBias();
    Vector3d getAcceBias();
    Vector3d getVIOffset();
    
    
    /* debug outputs */
    void printNominalState(bool is_full);
    void printErrorState(bool is_full);
    void printSlidingWindow();
    void printErrorCovariance(bool is_full);

    map<int, Vector3d> global_features;

};

#endif /* defined(__MyTriangulation__MSCKF__) */
