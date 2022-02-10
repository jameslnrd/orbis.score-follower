/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "orbis.score-follower.cpp"    // need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

SCENARIO("object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of our object") {

        test_wrapper<score_follower> an_instance;
        score_follower&              my_object = an_instance;

        // check that default attr values are correct
        REQUIRE((my_object.greeting == symbol("hello worldy")));

        // now proceed to testing various sequences of events
        WHEN("a 'bang' is received") {
            my_object.bang();
            THEN("our greeting is produced at the outlet") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE((output.size() == 1));
                REQUIRE((output[0].size() == 1));
                REQUIRE((output[0][0] == symbol("hello worldy")));
            }
        }
        
        /*
        // now proceed to testing various sequences of events
        WHEN("the defaults are used") {
            THEN("the input is the same as the output") {
                atoms input {1.0, 2.0, 3.0, 4.0};

                my_object.midi_in.anything(input);
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE((output.size() == 1));
                REQUIRE((output[0].size() == input.size()));
                for (auto i = 0; i < output.size(); ++i) {
                    REQUIRE((output[0][i] == input[i]));
                }
            }
        }
         */
    }
}
