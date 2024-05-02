import dependencies.urequests as requests
from time 	 import sleep

def get_current_time():

    #list of date time providers
    providers = ['https://timeapi.io/api/Time/current/zone?timeZone=America/Bogota',
                 'http://worldtimeapi.org/api/timezone/America/Bogota']

    fieldNames = ["dateTime", "datetime"]
    response = None

    for provider, fieldname in zip(providers, fieldNames):

        #retries quering the time
        for iretry in range(3):

            #try to ask to this particular time provider
            try:
                response = requests.request("GET", url = provider)#, timeout=10)
            #if any problem go to the next time provider
            except:
                sleep(1)
                continue

            if response == None:
                continue 

            elif (response.status_code == 200):
                parsed  = response.json()
                h, m, s = parse_datetime_time(parsed[fieldname])

                return (h, m, s)

    raise Exception("Sorry, seems like the time providers aren't working...")
    

def parse_datetime_time(datetime_str):
    
    start = 11
    end = -7
    
    txt = datetime_str.strip()
    txt = txt[start:end]
    time_str = txt.split(":")
    
    hour = int(time_str[0])
    minute = int(time_str[1])
    second = float(time_str[2])
    
    return hour, minute, second


class Time:
    def __init__(self, hour=0, min=0, sec=0):
        """
        Initializes a new Time object with the specified hour, minute, and second values.

        Args:
            hour (int): The hour value (0-23). Default is 0.
            min (int): The minute value (0-59). Default is 0.
            sec (int): The second value (0-59). Default is 0.
        """
        self.hour = hour
        self.min = min
        self.sec = sec

    @staticmethod
    def from_string(time_str):
        """
        Creates a new Time object from the specified string. With the format "HH:MM:SS".

        Args:
            time_str (str): The string to parse.

        Returns:
            Time: A new Time object.
        """
        hour, min, sec = time_str.split(":")
        return Time(int(hour), int(min), int(sec))
    
    @staticmethod
    def from_string_to_list(time_str):
        """
        Creates a new Time object from the specified string. With the format "HH:MM:SS".

        Args:
            time_str (str): The string to parse.

        Returns:
            Time: A new Time object.
        """
        hour, min, sec = time_str.split(":")
        return [int(hour), int(min), int(sec)]

    def to_total_minutes(self):
        """
        Converts the time to the total number of minutes.

        Returns:
            float: The total number of minutes.
        """
        total_minutes = (self.hour * 60) + (self.min) + (self.sec / 60)
        return total_minutes

    def to_total_seconds(self):
        """
        Converts the time to the total number of seconds.

        Returns:
            float: The total number of seconds.
        """
        total_seconds = (self.hour * 3600) + (self.min * 60) + self.sec
        return total_seconds

    def to_total_hours(self):
        """
        Converts the time to the total number of hours.

        Returns:
            float: The total number of hours.
        """
        total_hours = self.hour + (self.min / 60) + (self.sec / 3600)
        return total_hours

        
    def __gt__(self, other):
        """
        Overload the '>' operator to allow comparison between two Time objects.

        Args:
            other (Time): The Time object to compare with.

        Returns:
            bool: True if this Time object is greater than the specified Time object, False otherwise.
        """
        if self.hour > other.hour:
            return True
        elif self.hour == other.hour and self.min > other.min:
            return True
        elif self.hour == other.hour and self.min == other.min and self.sec > other.sec:
            return True
        else:
            return False

    def __ge__(self, other):
        """
        Overload the '>=' operator to allow comparison between two Time objects.

        Args:
            other (Time): The Time object to compare with.

        Returns:
            bool: True if this Time object is greater or equal than the specified Time object, False otherwise.
        """
        if self.hour > other.hour:
            return True
        elif self.hour == other.hour and self.min > other.min:
            return True
        elif self.hour == other.hour and self.min == other.min and self.sec >= other.sec:
            return True
        else:
            return False
        

    def __lt__(self, other):
        """
        Overload the '<' operator to allow comparison between two Time objects.

        Args:
            other (Time): The Time object to compare with.

        Returns:
            bool: True if this Time object is less than the specified Time object, False otherwise.
        """
        if self.hour < other.hour:
            return True
        elif self.hour == other.hour and self.min < other.min:
            return True
        elif self.hour == other.hour and self.min == other.min and self.sec < other.sec:
            return True
        else:
            return False
        
    def __eq__(self, other):
        """
        Overload the '==' operator to allow comparison between two Time objects.

        Args:
            other (Time): The Time object to compare with.

        Returns:
            bool: True if this Time object is equal to the specified Time object, False otherwise.
        """
        if self.hour == other.hour and self.min == other.min and self.sec == other.sec:
            return True
        else:
            return False
        
    def __sub__(self, other):
        """
        Overload the '-' operator to allow subtraction between two Time objects.

        Args:
            other (Time): The Time object to subtract from this Time object.

        Returns:
            Time: A new Time object representing the difference between this Time object and the specified Time object.
        """
        seconds1 = self.hour*3600 + self.min*60 + self.sec
        seconds2 = other.hour*3600 + other.min*60 + other.sec
        diff = seconds1 - seconds2
        
        # Calculate the new time components, and keep it within 24 hours and 60 minutes and 60 seconds
        new_hour = (diff // 3600) % 24
        remaining_seconds = diff % 3600
        new_min = remaining_seconds // 60
        new_sec = remaining_seconds % 60

        return Time(int(new_hour), int(new_min), int(new_sec))
        
    def __add__(self, other):
        """
        Overload the '+' operator to allow addition between two Time objects.

        Args:
            other (Time): The Time object to add to this Time object.

        Returns:
            Time: A new Time object representing the sum of this Time object and the specified Time object.
        """
        seconds1 = self.hour * 3600 + self.min * 60 + self.sec
        seconds2 = other.hour * 3600 + other.min * 60 + other.sec
        total_seconds = seconds1 + seconds2

        # Calculate the new time components, and keep it within 24 hours
        new_hour = (total_seconds // 3600) % 24
        remaining_seconds = total_seconds % 3600
        new_min = remaining_seconds // 60
        new_sec = remaining_seconds % 60

        return Time(int(new_hour), int(new_min), int(new_sec))

        
    def __str__(self):
        """
        Overload the str() function to print only the time in hh:mm:ss format.

        Returns:
            str: A string representing the time in hh:mm:ss format.
        """

        return "{:02d}:{:02d}:{:02d}".format(int(self.hour), int(self.min), int(self.sec))