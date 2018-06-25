#include <ros/ros.h>
#include <upo_msgs/PersonPoseArrayUPO.h>
#include <spencer_tracking_msgs/TrackedPersons.h>
#include <iostream>
#include <math.h>
//Visualization
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

class people_msg_transformer
{
private:
  ros::NodeHandle n_;
  ros::Subscriber sub_;
  ros::Publisher pub_;
  ros::Publisher people_arrow_pub_;
  ros::Publisher people_cyl_pub_;
  
public:
  people_msg_transformer()
  {}
  
  ~people_msg_transformer()
  {}
  
  void init()
  {
//     std::cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaa"<<std::endl;
    sub_ = n_.subscribe("/spencer/perception/tracked_persons_confirmed_by_HOG_or_upper_body", 1000, &people_msg_transformer::callback, this);
    pub_ = n_.advertise<upo_msgs::PersonPoseArrayUPO>("/people",1000);
    people_arrow_pub_ = n_.advertise<visualization_msgs::MarkerArray>("/people/people_arrow_markers", 1);
    people_cyl_pub_ = n_.advertise<visualization_msgs::MarkerArray>("/people/people_cylinders_markers", 1);
  }
  
  void callback(const spencer_tracking_msgs::TrackedPersons::ConstPtr& msg)
  {
//     std::cout<<"!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
//     spencer_tracking_msgs::TrackedPerson * Person;
    upo_msgs::PersonPoseArrayUPO people_upo;
    people_upo.header = msg->header;
    int size = msg->tracks.size();
    people_upo.size = size;
    for(int i=0;i<size;i++)
    {
      spencer_tracking_msgs::TrackedPerson people_spencer = msg->tracks[i];
      upo_msgs::PersonPoseUPO p_upo;
      p_upo.header = msg->header;
      p_upo.id = people_spencer.track_id;
      p_upo.position = people_spencer.pose.pose.position;
      p_upo.orientation = people_spencer.pose.pose.orientation;
      p_upo.vel = sqrt((people_spencer.twist.twist.linear.x * people_spencer.twist.twist.linear.x) + (people_spencer.twist.twist.linear.y * people_spencer.twist.twist.linear.y));
      people_upo.personPoses.push_back(p_upo);
    }
    
    if(size > 0)
    {
      pub_.publish(people_upo);
      visualize_people(people_upo);
    }
    
  }
  
  void visualize_people(upo_msgs::PersonPoseArrayUPO peop)
  {
    visualization_msgs::MarkerArray arrowArray;	
	visualization_msgs::MarkerArray cylinderArray;
	ros::Time time = ros::Time::now();
	for(unsigned int i=0; i<peop.size; i++)
	{
		visualization_msgs::Marker markerDel;
		//Delete previous markers
		markerDel.action = 3; //visualization_msgs::Marker::DELETEALL;
		arrowArray.markers.push_back(markerDel);
		cylinderArray.markers.push_back(markerDel);
		people_arrow_pub_.publish(arrowArray);
		people_cyl_pub_.publish(cylinderArray);

		// Fill up arrow marker information
		visualization_msgs::Marker marker;
		marker.header.frame_id = peop.header.frame_id;
		marker.header.stamp = time; 
		marker.ns = "people_pose";
		marker.id = (i+1);
		marker.type = visualization_msgs::Marker::ARROW;
		marker.action = visualization_msgs::Marker::ADD;
		marker.pose.position = peop.personPoses[i].position;
		marker.pose.orientation = peop.personPoses[i].orientation;
		marker.pose.position.z = 1.0; 
		marker.scale.x = 0.6; 
		marker.scale.y = 0.1; 
		marker.scale.z = 0.4; 
		marker.color.a = 0.8;
		marker.color.r = 0.0; 
		marker.color.g = 1.0; 
		marker.color.b = 0.0; 
		marker.lifetime = ros::Duration();
		arrowArray.markers.push_back(marker);

		// Fill up cylinder marker information
		marker.header.frame_id = peop.header.frame_id;
		marker.header.stamp = time; 
		marker.ns = "people_pose";
		marker.id = (i+1);
		marker.type = visualization_msgs::Marker::CYLINDER;
		marker.action = visualization_msgs::Marker::ADD;
		marker.pose.position = peop.personPoses[i].position;
		marker.pose.orientation = peop.personPoses[i].orientation;
		marker.pose.position.z = 0.50;
		marker.scale.x = 0.5; 
		marker.scale.y = 0.5; 
		marker.scale.z = 1.5; 
		marker.color.a = 0.8;
		marker.color.r = 0.0; 
		marker.color.g = 1.0; 
		marker.color.b = 0.0; 
		marker.lifetime = ros::Duration();
		cylinderArray.markers.push_back(marker);

	}
	people_arrow_pub_.publish(arrowArray);
	people_cyl_pub_.publish(cylinderArray);
  }
};


int main(int argc, char ** argv)
{
  ros::init(argc, argv, "peoplemsgs_spencer2upo");
  
  people_msg_transformer trans;
  trans.init();
  
  ros::spin();
  return 0;
}