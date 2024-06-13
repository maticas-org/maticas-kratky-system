#include <Arduino.h>
//#include "InternetRTC.h"

String updateConfigHTML(TimedOutput *actuators[], int size, InternetRTC &rtc) {
  String html = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Maticas línea decoración - Modulo 0</title><link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" rel=\"stylesheet\">";
  html += "<style>body {background-color: rgba(37, 212, 101, 0.856);display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;font-family: Arial, sans-serif;}";
  html += ".form-container {background-color: #fff;padding: 20px;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);text-align: center;width: 100%;max-width: 1400px;}";
  html += ".form-container img {width: 150px;margin-bottom: 20px;}";
  html += ".responsive-text h1 {font-size: calc(1.5rem + 1vw);}";
  html += ".responsive-text h3 {font-size: calc(1rem + 0.5vw);}";
  html += "table {width: 100%;}</style></head>";
  html += "<body><div class=\"container\"><div class=\"form-container mx-auto responsive-text\"><img src=\"https://raw.githubusercontent.com/maticas-org/maticas-web-page/c09a8c15c1e460f7b7f1edadfcc2fe08f04d301a/public/maticas-tech-logo.svg\" alt=\"Maticas Logo\" class=\"mx-auto d-block\">";
  html += "<h1>Maticas Tech línea Multipropósito</h1><h3>Módulo 0</h3><hr>";
  html += "<p>Hora local del micro controlador: " + rtc.getCurrentDateTime().toString() + "</p>";
  html += "<form action=\"/updateActuatorConf\" method=\"post\"><div class=\"form-group\"><label for=\"actuator-select\">Seleccionar actuador:</label><select id=\"actuator-select\" name=\"actuator\" class=\"form-control\">";
  
  // Add options for selecting each actuator
  for (int i = 0; i < size; i++) {
    html += "<option value=\"" + String(i) + "\">Actuator " + actuators[i]->name + "</option>";
  }

  html += "</select></div><div class=\"table-responsive\"><table id=\"config-table\" class=\"table\">";
  for (int i = 0; i < size; i++) {
    html += "<tr id=\"actuator-" + String(i) + "-config\" style=\"display: none;\">";
    html += "<td><div class=\"form-group\"><label for=\"nombre-actuador-" + String(i) + "\">Nombre de actuador</label>";
    html += "<input type=\"text\" id=\"nombre-actuador-" + String(i) + "\" name=\"name-" + String(i) + "\" value=\"" + actuators[i]->name + "\" class=\"form-control\"></div></td>";
    html += "<td><div class=\"form-group\"><label for=\"hora-inicio-" + String(i) + "\">Hora de inicio</label>";
    html += "<input type=\"time\" step=\"1\" id=\"hora-inicio-" + String(i) + "\" name=\"starttime-" + String(i) + "\" value=\"" + actuators[i]->onTime.toTimeString() + "\" class=\"form-control\"></div></td>";
    html += "<td><div class=\"form-group\"><label for=\"hora-fin-" + String(i) + "\">Hora de fin</label>";
    html += "<input type=\"time\" step=\"1\" id=\"hora-fin-" + String(i) + "\" name=\"endtime-" + String(i) + "\" value=\"" + actuators[i]->offTime.toTimeString() + "\" class=\"form-control\"></div></td>";
    html += "<td><div class=\"form-group\"><label for=\"tipo-actuador-" + String(i) + "\">Tipo de actuador</label>";
    html += "<select id=\"tipo-actuador-" + String(i) + "\" name=\"type-" + String(i) + "\" class=\"form-control\">";
    html += "<option value=\"on/off\"" + String(actuators[i]->mode == TimedOutput::ON_OFF ? " selected" : "") + ">On/Off</option>";
    html += "<option value=\"periodic\"" + String(actuators[i]->mode == TimedOutput::PERIODIC ? " selected" : "") + ">Periodico</option>";
    html += "</select></div></td>";
    html += "<td><div class=\"form-group\"><label for=\"minuteson-" + String(i) + "\">¿Cuántos segundos encendido?</label>";
    html += "<input type=\"number\" id=\"minuteson-" + String(i) + "\" name=\"minuteson-" + String(i) + "\" min=\"1.0\" step=\"any\" value=\"" + String(actuators[i]->onDuration) + "\" class=\"form-control\"></div></td>";
    html += "<td><div class=\"form-group\"><label for=\"minutesoff-" + String(i) + "\">¿Cuántos segundos apagado?</label>";
    html += "<input type=\"number\" id=\"minutesoff-" + String(i) + "\" name=\"minutesoff-" + String(i) + "\" min=\"1.0\" step=\"any\" value=\"" + String(actuators[i]->offDuration) + "\" class=\"form-control\"></div></td>";
    html += "</tr>";
  }

  html += "</table></div><div id=\"actuator-config-fields\"></div><br><button type=\"submit\" id=\"submit-btn\" class=\"btn btn-success\">Guardar</button></form></div></div>";
  html += "<script src=\"https://code.jquery.com/jquery-3.5.1.slim.min.js\"></script>";
  html += "<script src=\"https://cdn.jsdelivr.net/npm/@popperjs/core@2.5.4/dist/umd/popper.min.js\"></script>";
  html += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js\"></script>";
  html += "<script>document.getElementById('actuator-select').addEventListener('change', function () {";
  html += "var selectedActuator = this.value; var configFieldsDiv = document.getElementById('actuator-config-fields');";
  html += "configFieldsDiv.innerHTML = ''; var configFieldsHTML = '';";
  html += "configFieldsHTML += '<input type=\"hidden\" name=\"selected-actuator\" value=\"' + selectedActuator + '\">';";
  html += "configFieldsDiv.innerHTML = configFieldsHTML;";
  html += "var configTable = document.getElementById('config-table').rows;";
  html += "for (var i = 0; i < configTable.length; i++) {";
  html += "configTable[i].style.display = 'none';";
  html += "}";
  html += "document.getElementById('actuator-' + selectedActuator + '-config').style.display = 'table-row';";
  html += "});";
  html += "document.getElementById('submit-btn').addEventListener('click', function () {";
  html += "document.getElementById('submit-btn').disabled = true; document.forms[0].submit();";
  html += "});</script></body></html>";

  return html;
}


String updateWifiConfig(){
  String html = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Maticas línea decoración - Modulo 0</title><link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" rel=\"stylesheet\">";
  html += "<style>body {background-color: rgba(37, 212, 101, 0.856);display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;font-family: Arial, sans-serif;}";
  html += ".form-container {background-color: #fff;padding: 20px;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);text-align: center;width: 100%;max-width: 1400px;}";
  html += ".form-container img {width: 150px;margin-bottom: 20px;}";
  html += ".responsive-text h1 {font-size: calc(1.5rem + 1vw);}";
  html += ".responsive-text h3 {font-size: calc(1rem + 0.5vw);}";
  html += "table {width: 100%;}</style></head>";
  html += "<body><div class=\"container\"><div class=\"form-container mx-auto responsive-text\">";
  html += "<h1>Maticas Tech línea Multipropósito</h1><h3>Módulo 0</h3><hr>";
  html += "<p>¡Bienvenido a la configuración de red del módulo Maticas Tech 😄!</p>";
  html += "<p> Nos enorgullece que estés aquí, por favor ingresa los datos de tu red WIFI para continuar.</p>";
  html += "<p> Ante cualquier duda o problema, no dudes en <a href=\"http://maticas-tech.com/es/contactanos\">contactarnos</a>. Estamos para ayudarte.</p>";
  html += "<p> ¡Gracias por confiar en nosotros 🤝🫂!</p>";
  html += "<h1>Configuración de red</h1>";

  html += "<form action=\"/updateWifiConf\" method=\"post\">";
  html += "<div class=\"form-group\"><label for=\"ssid\">Nombre de la red WIFI</label>";
  html += "<input type=\"text\" id=\"ssid\" name=\"ssid\" class=\"form-control\"></div>";
  html += "<div class=\"form-group\"><label for=\"password\">Contraseña de la red WIFI</label>";
  html += "<input type=\"password\" id=\"password\" name=\"password\" class=\"form-control\"></div>";
  html += "<button type=\"submit\" class=\"btn btn-success\">Guardar</button></form></div></div>";

  html += "<script src=\"https://code.jquery.com/jquery-3.5.1.slim.min.js\"></script>";
  html += "<script src=\"https://cdn.jsdelivr.net/npm/@popperjs/core@2.5.4/dist/umd/popper.min.js\"></script>";
  html += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js\"></script></body></html>";

  return html;
}

String makingChangesHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html lang=\"es\">";
  html += "<head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>Maticas línea decoración - Modulo 0</title>";
  html += "<link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" rel=\"stylesheet\">";
  html += "<style>";
  html += "body {background-color: rgba(37, 212, 101, 0.856);display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;font-family: Arial, sans-serif;}";
  html += ".form-container {background-color: #fff;padding: 20px;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);text-align: center;width: 100%;max-width: 1400px;}";
  html += ".form-container img {width: 150px;margin-bottom: 20px;}";
  html += ".responsive-text h1 {font-size: calc(1.5rem + 1vw);}";
  html += ".responsive-text h3 {font-size: calc(1rem + 0.5vw);}";
  html += "table {width: 100%;}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class=\"container\">";
  html += "<div class=\"form-container mx-auto responsive-text\">";
  html += "<h1>Maticas Tech línea Multipropósito</h1>";
  html += "<h3>Módulo 0</h3>";
  html += "<hr>";
  html += "<p>¡Estamos realizando cambios en la configuración de tu módulo Maticas Tech! 😄</p>";
  html += "<p>Por favor espera un momento mientras se aplican los cambios. Usualmente toma menos de 1 minuto.</p>";
  html += "<hr>";
  html += "<p>Si cambiaste la configuración de red, es posible que debas reconectarte a la red WIFI del módulo <em>(\"Maticas Tech Module\")</em> si el proceso de conectar a la red WIFI que ingresaste falla.</p>";
  html += "<p>Si el proceso de conexión fue exitoso, puedes conectarte a tu red WIFI principal y acceder a la configuración del módulo en la dirección IP que muestra la pantalla :D.</p>";
  html += "<p>Ante cualquier duda o problema, no dudes en <a href=\"http://maticas-tech.com/es/contactanos\">contactarnos</a>. Estamos para ayudarte.</p>";
  html += "<p>¡Gracias por confiar en nosotros 🤝🫂!</p>";
  html += "</div>";
  html += "</div>";
  html += "<script src=\"https://code.jquery.com/jquery-3.5.1.slim.min.js\"></script>";
  html += "<script src=\"https://cdn.jsdelivr.net/npm/@popperjs/core@2.5.4/dist/umd/popper.min.js\"></script>";
  html += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js\"></script>";
  html += "</body>";
  html += "</html>";
  return html;
}
