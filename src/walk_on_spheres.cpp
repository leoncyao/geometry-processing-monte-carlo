#include "../include/walk_on_spheres.h"
#include <igl/AABB.h>
#include <igl/point_mesh_squared_distance.h>
#include <random>
#include <iostream>

using namespace std;
void walk_on_spheres(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const Eigen::VectorXd & B,
  const Eigen::MatrixXd & P,
  Eigen::VectorXd & U)
{
  // TODO: take stoping tolerance as parameter
    double eps = 10-2;

    igl::AABB<Eigen::MatrixXd, 3> tree;
    tree.init(V,F);

    Eigen::MatrixXd Q;
    Q.resizeLike(P);

    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;

    int iter = 0;

    // TODO: check for sqrD and early terminate the loop
    for(int i=0; i < P.rows(); i++){
      Q.row(i) = P.row(i);
    }
    
    while(iter < 5){
      iter++;
      // igl::point_mesh_squared_distance(Q, V, F, sqrD, I, C);
      tree.squared_distance(V,F,Q,sqrD,I,C);

      for(int i = 0; i < Q.rows(); i++){
        // sample a point on B(x)
        double radius = sqrt(sqrD(i));
        Eigen::RowVector3d center = Q.row(i);

        std::random_device rd;
        std::mt19937 generator (rd());
        std::uniform_real_distribution<double> uniform01(0.0, 1.0);

        //double M_PI = 3.14;
        double theta = 2 * 3.14 * uniform01(generator);
        //double phi = M_PI * uniform01(generator);
        //double x = sin(phi) * cos(theta);
        //double y = sin(phi) * sin(theta);
        //double z = cos(phi);

        double x = cos(theta);
        double y = sin(theta);

        //Eigen::RowVector3d sample(x, y, z);
        Eigen::RowVector3d sample(x, y, 0);
        sample = sample * radius + center;

        Q.row(i) = sample;
      }
    }

    // get closest face
    // igl::point_mesh_squared_distance(Q, V, F, sqrD, I, C);
    tree.squared_distance(V,F,Q,sqrD,I,C);


     U.resize(P.rows());

    for(int i=0; i < P.rows(); i++){

      // choose the closest vertex (TODO: maybe interpolate the values at the vertices of the closest face?)
      Eigen::RowVector3i face = F.row(I(i));
      int closest_idx = face(0);
      double closest_dist = (V.row(face(0)) - P.row(i)).squaredNorm();

      double dist = (V.row(face(1)) - P.row(i)).squaredNorm();
      if( dist < closest_dist){
        closest_idx = face(1);
        closest_dist = dist;
      }

      // dist = (V.row(face(2)) - P.row(i)).squaredNorm();
      //  if( dist < closest_dist){
      //   closest_idx = face(2);
      //   closest_dist = dist;
      // }


      // get the boundary value
      U(i) = B(closest_idx);
      // std::cout << closest_dist << " " << closest_idx << " " << U(i) << std::endl;
    }

    
}