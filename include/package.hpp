#pragma once
#include <string>

enum class Content_type{
    FLUID_NON_FOOD=1,
    SOLID_NON_FOOD=2,
    PROCESSED_FOOD=3,
    PROCESSED_DRINK=4,
    MEDICINE_OR_CHEMICALS=5
};

enum class package_status{
    ORDER_RECEIVED=1,
    PROCESSING=2,
    PROCESSED=3,
    IN_TRANSIT=4,
    DELIVERED=5   
};

class package{

    private:
        std::string package_name;
        std::string sender_name;
        std::string receiver_name;
        std::string package_id;
        Content_type content_type;
        package_status status;
        size_t priority=0;
        double weight;

        // Helper method to determine priority automatically from Content_type
        static size_t calculate_priority(Content_type type);
        
        //id should be generated randomly than assigning count, Helper method that generates random package ID upto 10 characters starting with PKG-
        static std::string generateID();
            
    
    public:

        package(const std::string& pN,const std::string& sN, const std::string& rN, double w, const Content_type cont)
        : package_name(pN), sender_name(sN), receiver_name(rN), weight(w), content_type(cont){

            status=package_status::ORDER_RECEIVED;
            package_id=generateID();
            priority = calculate_priority(content_type);

        }

        //getters
        std::string getID()const {return package_id;}
        Content_type getContentType() const {return content_type;}
        package_status getStatus() const {return status;}
        double getWeight()const{return weight;}
        size_t getPriority()const{return priority;}

        //setters
        void setContentType(const Content_type& newtype){
            content_type=newtype;
            priority=calculate_priority(newtype);
        }
        void setStatus(const package_status& newstatus){status=newstatus;}
        void setWeight(double newWeight){weight=newWeight;}

        void set_priority(size_t newPriority){priority=newPriority;}
        
        //method that gives package summary
        std::string getSummary()const;
 
};