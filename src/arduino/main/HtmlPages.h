#include <Arduino.h>




String updateConfigHTML(TimedOutput *actuators[], int size) {
  String html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Maticas linea decoración - Modulo 0</title><link href=\"https://templatemo.com/tm-style-20210719c.css\" rel=\"stylesheet\"></head><body><div class=\"form-container\"><h1>Maticas linea decoración</h1><h3>Modulo 0</h3><hr><form action=\"/updateActuatorConf\" method=\"post\"><label for=\"actuator-select\">Seleccionar actuador:</label><select id=\"actuator-select\" name=\"actuator\">";
  
  // Add options for selecting each actuator
  for (int i = 0; i < size; i++) {
    html += "<option value=\"" + String(i) + "\">Actuator " + actuators[i]->name + "</option>";
  }

  html += "</select><br><br>";

  // Add fields for displaying configuration of selected actuator
  html += "<table id=\"config-table\">";
  for (int i = 0; i < size; i++) {
    html += "<tr id=\"actuator-" + String(i) + "-config\" style=\"display: none;\">";
    html += "<td><label for=\"nombre-actuador-" + String(i) + "\">Nombre de actuador</label>";
    html += "<input type=\"text\" id=\"nombre-actuador-" + String(i) + "\" name=\"name-" + String(i) + "\" value=\"" + actuators[i]->name + "\"></td>";
    html += "<td><label for=\"hora-inicio-" + String(i) + "\">Hora de inicio</label>";
    html += "<input type=\"time\" step=\"1\" id=\"hora-inicio-" + String(i) + "\" name=\"starttime-" + String(i) + "\" value=\"" + actuators[i]->onTime.toTimeString() + "\"></td>";
    html += "<td><label for=\"hora-fin-" + String(i) + "\">Hora de fin</label>";
    html += "<input type=\"time\" step=\"1\" id=\"hora-fin-" + String(i) + "\" name=\"endtime-" + String(i) + "\" value=\"" + actuators[i]->offTime.toTimeString() + "\"></td>";
    html += "<td><label for=\"tipo-actuador-" + String(i) + "\">Tipo de actuador</label>";
    html += "<select id=\"tipo-actuador-" + String(i) + "\" name=\"type-" + String(i) + "\">";
    html += "<option value=\"on/off\"" + String(actuators[i]->mode == TimedOutput::ON_OFF ? " selected" : "") + ">On/Off</option>";
    html += "<option value=\"periodic\"" + String(actuators[i]->mode == TimedOutput::PERIODIC ? " selected" : "") + ">Periodico</option>";
    html += "</select></td>";
    html += "<td><label for=\"minuteson-" + String(i) + "\">¿Cuántos minutos encendido?</label>";
    html += "<input type=\"number\" id=\"minuteson-" + String(i) + "\" name=\"minuteson-" + String(i) + "\" min=\"1.0\" step=\"any\" value=\"" + String(actuators[i]->onDuration) + "\"></td>";
    html += "<td><label for=\"minutesoff-" + String(i) + "\">¿Cuántos minutos apagado?</label>";
    html += "<input type=\"number\" id=\"minutesoff-" + String(i) + "\" name=\"minutesoff-" + String(i) + "\" min=\"1.0\" step=\"any\" value=\"" + String(actuators[i]->offDuration) + "\"></td>";
    html += "</tr>";
  }

  // Add fields for displaying configuration of selected actuator
  html += "<div id=\"actuator-config-fields\"></div>";
  html += "<br><input type=\"submit\" id=\"submit-btn\" value=\"Guardar\"></form></div>";

  // JavaScript to handle dropdown selection
  html += "<script>";
  html += "document.getElementById('actuator-select').addEventListener('change', function() {";
  html += "var selectedActuator = this.value;";
  html += "var configFieldsDiv = document.getElementById('actuator-config-fields');";
  html += "configFieldsDiv.innerHTML = '';"; // Clear existing fields
  html += "var configFieldsHTML = '';";
  // Add input fields for the selected actuator
  html += "configFieldsHTML += '<input type=\"hidden\" name=\"selected-actuator\" value=\"' + selectedActuator + '\">';";
  // You can add more input fields here based on the selected actuator if needed
  html += "configFieldsDiv.innerHTML = configFieldsHTML;";
  html += "});";
  html += "</script>";
  html += "</body></html>";
  
  // Add JavaScript to toggle visibility of configuration fields based on selected actuator
  //html += "<script>document.getElementById('actuator-select').addEventListener('change', function() { var selectedActuator = this.value; var configTable = document.getElementById('config-table').rows; for (var i = 0; i < configTable.length; i++) { configTable[i].style.display = 'none'; } document.getElementById('actuator-' + selectedActuator + '-config').style.display = 'table-row'; });</script>";
  html += "<script>document.getElementById('actuator-select').addEventListener('change', function() { var selectedActuator = this.value; var configTable = document.getElementById('config-table').rows; for (var i = 0; i < configTable.length; i++) { configTable[i].style.display = 'none'; } document.getElementById('actuator-' + selectedActuator + '-config').style.display = 'table-row'; });";
  html += "document.getElementById('submit-btn').addEventListener('click', function() { document.getElementById('submit-btn').disabled = true; document.forms[0].submit(); });</script>";

  return html;
}


