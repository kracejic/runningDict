# copy all from data to install dir /share
# here can be images, models

# install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/images
#         DESTINATION ./share/images
#     )

# install all data
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dicts/ DESTINATION share/runningDict )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/share/ DESTINATION share)

if(MINGW)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dataWindows/ DESTINATION .  )
endif(MINGW)
