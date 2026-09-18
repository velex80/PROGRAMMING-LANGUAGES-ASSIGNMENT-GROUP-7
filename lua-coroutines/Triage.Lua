-- COUNTY HOSPITAL EMERGENCY TRIAGE SYSTEM
-- Question 7
-- Lua 5.5
-- Uses coroutines for independent patient workflows

-- ============================================================
-- 1. PRIORITY DEFINITIONS
-- ============================================================

local PRIORITY = {
    Routine = 1,
    Urgent = 2,
    Critical = 3
}


-- ============================================================
-- 2. PATIENT WORKFLOW
-- Each patient has an independent coroutine.
-- The coroutine yields after every treatment stage.
-- ============================================================

function patientWorkflow(patient)

    -- Stage 1: Registration
    print(patient.name .. " -> Registration")
    coroutine.yield("Registration completed")


    -- Stage 2: Triage
    print(patient.name .. " -> Triage")
    coroutine.yield("Triage completed")


    -- Stage 3: Consultation
    print(patient.name .. " -> Consultation")
    coroutine.yield("Consultation completed")


    -- Stage 4: Discharge
    print(patient.name .. " -> Discharge")
    coroutine.yield("Discharge completed")


    print(patient.name .. " -> Treatment finished")
end


-- ============================================================
-- 3. FICTIONAL PATIENT DATA
-- ============================================================

local patients = {

    {
        name = "Patient A",
        type = "Routine",
        priority = PRIORITY.Routine,
        waiting = 0,
        arrived = true
    },

    {
        name = "Patient B",
        type = "Urgent",
        priority = PRIORITY.Urgent,
        waiting = 0,
        arrived = true
    },

    {
        name = "Patient C",
        type = "Critical",
        priority = PRIORITY.Critical,
        waiting = 0,
        arrived = true
    }
}


-- ============================================================
-- 4. CREATE AN INDEPENDENT COROUTINE FOR EACH PATIENT
-- ============================================================

for _, patient in ipairs(patients) do

    patient.co = coroutine.create(function()
        patientWorkflow(patient)
    end)

end


-- ============================================================
-- 5. EFFECTIVE PRIORITY
--
-- Aging is used to prevent starvation.
--
-- Effective Priority =
-- Base Priority + Waiting Time
-- ============================================================

function effectivePriority(patient)

    return patient.priority + patient.waiting

end


-- ============================================================
-- 6. SELECT THE NEXT PATIENT
-- The scheduler chooses the patient with the highest
-- effective priority.
-- Dead coroutines are ignored.
-- ============================================================

function selectPatient(patients)

    local selected = nil

    for _, patient in ipairs(patients) do

        -- Never select a patient whose coroutine is dead
        if coroutine.status(patient.co) ~= "dead" then

            if selected == nil or
               effectivePriority(patient) >
               effectivePriority(selected) then

                selected = patient

            end

        end

    end

    return selected
end


-- ============================================================
-- 7. DISPLAY INITIAL PATIENT INFORMATION
-- ============================================================

print("==============================================")
print(" COUNTY HOSPITAL EMERGENCY TRIAGE SYSTEM")
print("==============================================")

print("\nPatients registered:")

for _, patient in ipairs(patients) do

    print(
        patient.name ..
        " | " ..
        patient.type ..
        " | Priority: " ..
        patient.priority
    )

end


-- ============================================================
-- 8. DISPLAY INITIAL COROUTINE STATES
-- ============================================================

print("\nInitial coroutine states:")

for _, patient in ipairs(patients) do

    print(
        patient.name ..
        ": " ..
        coroutine.status(patient.co)
    )

end


-- ============================================================
-- 9. HOSPITAL SCHEDULER
-- ============================================================

print("\n==============================================")
print(" STARTING HOSPITAL SCHEDULER")
print("==============================================")


local completed = 0
local round = 1

while completed < #patients do

    print("\n---------- Scheduling Round " .. round .. " ----------")


    -- --------------------------------------------------------
    -- Select patient according to priority + aging
    -- --------------------------------------------------------

    local selected = selectPatient(patients)


    -- Safety check
    if selected == nil then

        print("No available patients.")
        break

    end


    print(
        "Scheduler selected: " ..
        selected.name ..
        " (" ..
        selected.type ..
        ")"
    )

    print(
        "Effective priority: " ..
        effectivePriority(selected)
    )


    -- --------------------------------------------------------
    -- Check coroutine state before resuming
    -- --------------------------------------------------------

    local status = coroutine.status(selected.co)

    print("Coroutine status: " .. status)


    if status ~= "dead" then

        -- ----------------------------------------------------
        -- Resume the selected patient's workflow
        -- ----------------------------------------------------

        local success, message =
            coroutine.resume(selected.co)


        -- ----------------------------------------------------
        -- Error handling
        -- ----------------------------------------------------

        if success then

            print(
                "Scheduler received control back from " ..
                selected.name
            )

        else

            print(
                "ERROR processing " ..
                selected.name
            )

            print("Error message: " .. tostring(message))

        end

    else

        print(
            selected.name ..
            " cannot be resumed because its coroutine is dead."
        )

    end


    -- --------------------------------------------------------
    -- Update waiting times for patients that were not selected
    -- This is the AGING mechanism.
    -- --------------------------------------------------------

    for _, patient in ipairs(patients) do

        if patient ~= selected and
           coroutine.status(patient.co) ~= "dead" then

            patient.waiting = patient.waiting + 1

        end

    end


    -- The selected patient has just received service.
    selected.waiting = 0


    -- --------------------------------------------------------
    -- Display current coroutine states
    -- --------------------------------------------------------

    print("\nCurrent patient states:")

    for _, patient in ipairs(patients) do

        print(
            patient.name ..
            " | " ..
            patient.type ..
            " | Waiting: " ..
            patient.waiting ..
            " | Coroutine: " ..
            coroutine.status(patient.co)
        )

    end


    -- --------------------------------------------------------
    -- Count completed patients
    -- --------------------------------------------------------

    completed = 0

    for _, patient in ipairs(patients) do

        if coroutine.status(patient.co) == "dead" then

            completed = completed + 1

        end

    end


    round = round + 1

end


-- ============================================================
-- 10. FINAL RESULTS
-- ============================================================

print("\n==============================================")
print(" ALL PATIENT WORKFLOWS COMPLETED")
print("==============================================")

for _, patient in ipairs(patients) do

    print(
        patient.name ..
        " -> Final coroutine status: " ..
        coroutine.status(patient.co)
    )

end

print("\nHospital scheduler finished successfully.")