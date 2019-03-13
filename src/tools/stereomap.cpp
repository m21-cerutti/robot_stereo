﻿#include "tools/stereomap.h"



void StereoMap::computeBMDisparity(const cv::Mat &src, cv::Mat &out, cv::Ptr<cv::StereoBM> bm_state)
{
    cv::Mat left_mat, right_mat;

    ///Separate
    ImageFilter::separateImage(src, left_mat, right_mat);

    computeBMDisparityStereo(left_mat, right_mat, out, bm_state);
}

void StereoMap::computeBMDisparityStereo(const cv::Mat &src_left, const cv::Mat &src_right, cv::Mat &out, cv::Ptr<cv::StereoBM> bm_state)
{
    cv::Mat invert, left_mat, right_mat, disparity;

    ///Gray
    ImageFilter::applyGray(src_left, left_mat);
    ImageFilter::applyGray(src_right, right_mat);

    ///Disparity map
    bm_state->compute(left_mat, right_mat, disparity);
    ImageFilter::fillBlank(disparity, invert);
    cv::subtract(invert, disparity, disparity);

    disparity.convertTo(disparity, CV_32F);


    disparity.copyTo(out);
}

void StereoMap::computeSGBMDisparity(const cv::Mat &src, cv::Mat &out, cv::Ptr<cv::StereoSGBM> sgbm_state)
{
    cv::Mat left_mat, right_mat;
    ImageFilter::separateImage(src, left_mat, right_mat);

    computeSGBMDisparityStereo(left_mat, right_mat, out, sgbm_state);
}

void StereoMap::computeSGBMDisparityStereo(const cv::Mat &src_left, const cv::Mat &src_right, cv::Mat &out, cv::Ptr<cv::StereoSGBM> sgbm_state)
{
    cv::Mat invert, left_mat, right_mat, disparity;

    ///Gray
    ImageFilter::applyGray(src_left, left_mat);
    ImageFilter::applyGray(src_right, right_mat);

    ///Disparity map
    sgbm_state->compute(left_mat, right_mat, disparity);
    ImageFilter::fillBlank(disparity, invert);
    cv::subtract(invert, disparity, disparity);

    disparity.convertTo(disparity, CV_32F);


    disparity.copyTo(out);
}

void StereoMap::computeDepthMap(const cv::Mat &disparity, const cv::Mat &Q, cv::Mat &depth_map, float depth_min, float depth_max)
{
    cv::Mat image_3d;
    cv::reprojectImageTo3D(disparity, image_3d, Q, true, CV_32F);

    //cv::normalize(image_3d, image_3d, 0, 255, CV_MINMAX, CV_8UC1);
    //cv::threshold( image_3d, image_3d, 0, 1, CV_THRESH_TRUNC );

    depth_map = cv::Mat::zeros(image_3d.rows, image_3d.cols, CV_32FC1);

    for(int i = 0; i < image_3d.rows; i++) {
        for(int j = 0; j < image_3d.cols; j++) {
            cv::Vec3f point = image_3d.at<cv::Vec3f>(i,j);
            float z = - point[2];
            if( z >= depth_min && z <= depth_max){
                depth_map.at<float>(i,j) = z;
            }
            else if (z > depth_max)
            {
                depth_map.at<float>(i,j) = depth_max+1;
            }
            else if (z < depth_min)
            {
                depth_map.at<float>(i,j) = depth_min-1;
            }
        }
    }


}