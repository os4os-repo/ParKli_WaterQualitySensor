function decodeUplink(input) {
  
    var bytes = input.bytes;
    var decoded = {Batterie:(batteryLvl((bytes[0]<< 8) + bytes[1])),Leitwert:((bytes[2]<< 8) + bytes[3]),Truebung:turbidity(((bytes[4]<< 8) + bytes[5])),PH:ph(((bytes[6]<< 8) + bytes[7])),Temperatur_1_Meter:temperatur(((bytes[8]<< 8) + bytes[9])),Temperatur_2_Meter:temperatur(((bytes[10]<< 8) + bytes[11])),Temperatur_3_Meter:temperatur(((bytes[12]<< 8) + bytes[13])),Aufruf:((bytes[14]<< 8) + bytes[15])};
    
    return {
      data: decoded,
      warnings: [],
      errors: []
    };
  }
  
  
  function batteryLvl(input){
    input = (input-500) / 5; 
    input = Math.round(input * 100) / 100; 
    return input +"%";
  }
  
  function turbidity(input){
    //input ist zwischen 0 = 0% und 1000 = 1000%
    input/=10;
   // input = Math.round(input * 100) / 100;
    return input +"%";
  }
  
  function temperatur(input){
    input = input/=100
    
    return input +"°C";
  }
  
  function ph(input){
    //15 PH Werte über einen bereich von 0-1024 verteilt
    input = input/=(1024/15);
    input = Math.round(input * 100) / 100; 
    return input +"";
  }