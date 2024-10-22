-- Keep a log of any SQL queries you execute as you solve the mystery.

-- crime_scene_reports, with the date of the crime and the place, might tell us more about the crime itself -> has a column "description" which i might want to figure out what is
SELECT description FROM crime_scene_reports
WHERE month = 7 AND day = 28 AND street = 'Humphrey Street';
-- DESCRIPTION: Theft of the CS50 duck took place at 10:15am at the Humphrey Street bakery.
--              Interviews were conducted today with three witnesses who were present at the time – each of their interview transcripts mentions the bakery.
--              Littering took place at 16:36. No known witnesses.

-- let's look into the interviews so see the name and transcript. Also let's select the transcripts that mention the word 'thief'. Let's also select year to be sure they all were conducted in the same year.
SELECT name, year, transcript FROM interviews
WHERE month = 7 AND day = 28 AND transcript LIKE '%thief%';
-- Indeed, there are 3 of them, all in year 2023. The names of the witnesses are:

--              + Ruth     : Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away.
--                            If you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time frame.
--
--                              --> Might want to look at bakery_security_logs from 10:16 to 10:25

--              + Eugene   : I don't know the thief's name, but it was someone I recognized.
--                            Earlier this morning, before I arrived at Emma's bakery, I was walking by the ATM on Leggett Street and saw the thief there withdrawing some money.

--                              --> 1. Emma is new suspect
--                                  2. Look at ATM transactions before 10:15

--              + Raymond. : As the thief was leaving the bakery, they called someone who talked to them for less than a minute. In the call, I heard the thief say that they were
--                            planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked the person on the other end of the phone to purchase the flight ticket.

--                              --> 1. Look at phone_calls on 07/28 with duration of less than 1 minute
--                                  2. Thief and call receiver planned to take the first flight on the next day (07/29) from Fiftyville
--                                  3. Thief (caller) asked the other person to purchase the ticket

-- Before making implication from this query, let's create a list of suspects:

-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  Three witnesses:
--      1. Ruth
--      2. Eugene
--      3. Raymond

--  Owner of the bakery
--      4. Emma

-- *************************************
-- *************************************


-- Now let's invetigate the transcripts:

-- 1. Ruth's transcript: looking for cars leaving between 10:15 and 10:25 included. returns several license_plate which identify vehicles and that can be connected to table people:
SELECT people.name FROM people
JOIN bakery_security_logs ON people.license_plate = bakery_security_logs.license_plate
WHERE bakery_security_logs.hour = 10 AND bakery_security_logs.month = 7 AND bakery_security_logs.day = 28 AND bakery_security_logs.minute > 15 AND bakery_security_logs.minute <= 25 AND activity = 'exit' AND year = 2023;
-- returns a list of the people leaving the bakery parking lot between 10:15 and 10:25 included
-- Let's update the list of suspects. Witnesses and bakery owner cannot be considered suspects anymore


-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  People (license_plate) leaving the
--  bakery parking lot bwtween 10:15 and
--  10:25 included

--      1. Vanessa
--      2. Bruce
--      3. Barry
--      4. Luca
--      5. Iman
--      6. Diana
--      7. Kelsey

-- *************************************
-- *************************************

-- 2. Eugene recognises the thief as she saw him/her earlier that morning at the Leggett Street atm withdrawing money. This can lead us to bank accounts and therefore to list of people
SELECT people.name FROM people
JOIN bank_accounts ON people.id = bank_accounts.person_id
JOIN atm_transactions ON bank_accounts.account_number = atm_transactions.account_number
WHERE atm_transactions.year = 2023 AND atm_transactions.month = 7 AND atm_transactions.day = 28 AND atm_transactions.atm_location = 'Leggett Street' AND atm_transactions.transaction_type = 'withdraw' AND people.name IN(

    SELECT people.name FROM people
    JOIN bakery_security_logs ON people.license_plate = bakery_security_logs.license_plate
    WHERE bakery_security_logs.hour = 10 AND bakery_security_logs.month = 7 AND bakery_security_logs.day = 28 AND bakery_security_logs.minute > 15 AND bakery_security_logs.minute <= 25 AND activity = 'exit' AND year = 2023);
-- return list of people who have withdrawn money on 7.28.2023 and that match the list of people leaving the bakery parking lot between 10:15 and 10:25 included
-- let's update the list of suspects and let's add a section for the accomplice

-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  + THIEF
--  people who have withdrawn money on
--  7.28.2023 and that match the list of
--  people leaving the bakery parking
--  lot between 10:15 and 10:25 included

--      1. Bruce
--      2. Diana
--      3. Iman
--      4. Luca

--  + ACCOMPLICE

-- *************************************
-- *************************************


-- 3. Raymond

-- Now let's use the information about the purchase of the flight. First flight on 7.29 from Fiftyville. Minimize hour first and then minutes
-- to find the earliest flight. Also specify the day 7.29 and the airport origin id that is LIKE Fiftyville.

SELECT people.name FROM people
JOIN passengers ON people.passport_number = passengers.passport_number
JOIN flights ON passengers.flight_id = flights.id
JOIN airports ON flights.origin_airport_id = airports.id
WHERE airports.full_name LIKE '%Fiftyville%' AND flights.year = 2023 AND flights.month = 7 AND flights.day = 29
AND flights.minute = (

    SELECT MIN(flights.minute) FROM flights
    JOIN airports ON flights.origin_airport_id
    WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%' AND flights.hour = (

            SELECT MIN(flights.hour) FROM flights
            JOIN airports ON flights.origin_airport_id
            WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%'))

AND flights.hour = (
    SELECT MIN(flights.hour) FROM flights
    JOIN airports ON flights.origin_airport_id
    WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%'
);

-- This query returns the following names:

-- +--------+
-- | Doris  |
-- | Sofia  |
-- | Bruce  |
-- | Edward |
-- | Kelsey |
-- | Taylor |
-- | Kenny  |
-- | Luca   |
-- +--------+

-- Which decreses the number of suspects for the as THIEF

-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  + THIEF
--  people who have withdrawn money on
--  7.28.2023 and that match the list of
--  people leaving the bakery parking
--  lot between 10:15 and 10:25 included
--  and that called someone between on 7.28
--  to tell their accomplice to buy the first
--  ticket on the next day out of FIftiville.

--      1. Bruce
--      2. Luca

--  + ACCOMPLICE

-- *************************************
-- *************************************

-- With Raymon'd transcript we can also obtain information about the accomplice. We look for calls with the caller number corresponding to the suspects' names.
-- Also, the call was amd on 7.28 after 10:15 and lasted less than 1 minute
-- This returns the number of the receiver that can be use to obtain a list of potential accomplices.

SELECT name FROM people
WHERE phone_number IN (

    SELECT phone_calls.receiver FROM phone_calls
    JOIN people ON people.phone_number = phone_calls.caller
    WHERE duration < 60 AND year = 2023 AND month = 7 AND day = 28 AND caller IN (

        SELECT people.phone_number FROM people
        WHERE people.name IN ('Bruce', 'Luca'))
);

-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  + THIEF
--  people who have withdrawn money on
--  7.28.2023 and that match the list of
--  people leaving the bakery parking
--  lot between 10:15 and 10:25 included
--  and that called someone between on 7.28
--  to tell their accomplice to buy the first
--  ticket on the next day out of FIftiville.

--      1. Bruce
--      2. Luca

--  + ACCOMPLICE
--  receved call from thief and purchased the
--  earliest flight on 7.29 from Fiftyville

--      1. Robin

-- *************************************
-- *************************************

-- now let's see what we can find out about the city the thief escaped to
SELECT id, full_name, city FROM airports
WHERE id IN (
    SELECT flights.destination_airport_id FROM flights
    JOIN airports ON flights.origin_airport_id = airports.id
    WHERE airports.full_name LIKE '%Fiftyville%' AND flights.year = 2023 AND flights.month = 7 AND flights.day = 29
    AND flights.minute = (

        SELECT MIN(flights.minute) FROM flights
        JOIN airports ON flights.origin_airport_id
        WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%' AND flights.hour = (

                SELECT MIN(flights.hour) FROM flights
                JOIN airports ON flights.origin_airport_id
                WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%')
        )

    AND flights.hour = (
        SELECT MIN(flights.hour) FROM flights
        JOIN airports ON flights.origin_airport_id
        WHERE flights.year = 2023 AND flights.month = 7 AND flights.day = 29 AND airports.full_name LIKE '%Fiftyville%')
);


-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  + THIEF
--  people who have withdrawn money on
--  7.28.2023 and that match the list of
--  people leaving the bakery parking
--  lot between 10:15 and 10:25 included
--  and that called someone between on 7.28
--  to tell their accomplice to buy the first
--  ticket on the next day out of FIftiville.

--      1. Bruce
--      2. Luca

--  + ACCOMPLICE
--  receved call from thief and purchased the
--  earliest flight on 7.29 from Fiftyville

--      1. Robin

--  + DESTINATION

--      1. LaGuardia Airport - New York City

-- *************************************
-- *************************************

-- To find out the theif's name, let's look for the call-caller(s) where the receiver is Robin on 7.28 and duratio n < 60
SELECT name FROM people
WHERE phone_number IN (

    SELECT phone_calls.caller FROM phone_calls
    JOIN people ON people.phone_number = phone_calls.receiver
    WHERE duration < 60 AND year = 2023 AND month = 7 AND day = 28 AND receiver IN (

        SELECT people.phone_number FROM people
        WHERE people.name = 'Robin')
);


-- *************************************
-- ******** SUSPECT LIST UPDATE ********
-- *************************************

--  + THIEF

--      1. Bruce

--  + ACCOMPLICE

--      1. Robin

--  + DESTINATION

--      1. New York City

-- *************************************
-- *************************************
