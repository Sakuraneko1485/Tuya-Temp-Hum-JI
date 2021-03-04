
//U8x8模式下12864显示面积为16x8
void oled_display(){
  u8x8.setPowerSave(0);            //1-启用节能模式，0-关闭节能模式
  wifi_display();                  //WiFi连接状态指示
  bat_display();                   //电池状态指示
  temp_display();                  //显示温度
  hum_display();                   //显示湿度
}

//电池情况显示
void bat_display(){
  //当电池电量在20%以上时显示满电图标
  if (Bat >= 20){
    u8x8.setFont(u8x8_font_open_iconic_embedded_2x2);
    u8x8.drawGlyph(3, 0, '@'+9);
  }
  
  //当电池电量在20%以下时显示欠电图标，提醒电量不足
  //每秒闪烁一次电池图标
  else {
    
    if (millis() - display_time >= 1000){
      display_time = millis();
      u8x8.setFont(u8x8_font_open_iconic_embedded_2x2);
      u8x8.drawGlyph(3, 0, '@');
      delay(1000);
      u8x8.setFont(u8x8_font_open_iconic_embedded_2x2);
      u8x8.drawGlyph(3, 0, '@'+9);
    }
  }
  //显示电池电压
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.setCursor(9,0);
  u8x8.print(Voltage);         
  u8x8.drawString(15,0,"V");
}

void wifi_display(){
  //连接上WiFi后，显示WiFi图标
  if (wifi_connectstate = 2){
    u8x8.setFont(u8x8_font_open_iconic_embedded_2x2);
    u8x8.drawGlyph(0, 0, '@'+16);
  }
  //未连接WiFi，显示X图标
  else{
    u8x8.setFont(u8x8_font_open_iconic_check_2x2);
    u8x8.drawGlyph(0, 0, '@'+2);
  }
}

  //显示温度
void temp_display(){
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.drawString(0,3,"T:");
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setCursor(3,2);
  u8x8.print(Sht30_Temp);
  u8x8.drawString(14,2,"C");

}

   //显示湿度
void hum_display(){
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.drawString(0,6,"RH:");
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setCursor(3,5);
  u8x8.print(Sht30_Hum);
  u8x8.drawString(14,5,"%");
}
